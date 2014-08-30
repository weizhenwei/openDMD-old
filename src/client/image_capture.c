/*
 *
 * Copyright (c) 2014, weizhenwei
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the {organization} nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: image_capture.c
 *
 * Brief: capture image from video device. 
 *
 * Date: 2014.05.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "image_capture.h"

#include <assert.h>
#include <errno.h>
#include <linux/limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include "global_context.h"
#include "image_convert.h"
#include "log.h"
#include "picture_thread.h"
#include "signal_handler.h"
#include "sqlite_utils.h"
#include "statistics.h"
#include "video_thread.h"


static void notify_picture()
{
    dmd_log(LOG_DEBUG, "in %s, notify to picture thread\n", __func__);
    pthread_mutex_lock(&global.client.thread_attr.picture_mutex);
    global.client.picture_target = NOTIFY_PICTURE;
    pthread_cond_signal(&global.client.thread_attr.picture_cond);
    pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);
}

static void notify_video()
{
    dmd_log(LOG_DEBUG, "in %s, notify to video thread\n", __func__);
    pthread_mutex_lock(&global.client.thread_attr.video_mutex);
    global.client.video_target = NOTIFY_VIDEO;
    pthread_cond_signal(&global.client.thread_attr.video_cond);
    pthread_mutex_unlock(&global.client.thread_attr.video_mutex);
}

int process_image(void *yuyv, int length, int width, int height)
{
    int ret = -1;

    assert(length == width * height * 2);

    // motion detection;
    ret = YUYV422_motion_detect((uint8_t *)yuyv, width, height, length);

    time_t now;
    now = time(&now);
    assert(now != -1);
    if (ret == 0) { // motion detected!
        
        dmd_log(LOG_INFO, "motion detected !\n");

        // if this detection is the first capture in current motion,
        // create struct motion_t variable;
        pthread_mutex_lock(&global_stats->mutex);
        if (global_stats->current_motion == NULL) {
            global_stats->current_motion = new_motion();
            set_motion_start_time(global_stats->current_motion, now);
        }
        pthread_mutex_unlock(&global_stats->mutex);

        // first refresh detection time;
        if (global.client.working_mode == CAPTURE_PICTURE
                || global.client.working_mode == CAPTURE_ALL) {
            // associated with jpeg filename;
            if (global.client.lasttime == now) {
                global.client.counter_in_second++;
            } else {
                global.client.counter_in_second = 0;
            }
        }
        global.client.lasttime = now;

        // then copy image to buffer, remember to lock it first;
        pthread_rwlock_wrlock(&global.client.thread_attr.bufferYUYV_rwlock);
        memcpy(global.client.bufferingYUYV422, yuyv, length);
        pthread_rwlock_unlock(&global.client.thread_attr.bufferYUYV_rwlock);

        // notify picture thread and/or video thread;
        if (global.client.working_mode == CAPTURE_ALL) {
            // notify all;
            notify_picture();
            notify_video();
        } else if (global.client.working_mode == CAPTURE_PICTURE) {
            // only notify picture thread;
            notify_picture();
        } else if (global.client.working_mode == CAPTURE_VIDEO) {
            // only notify video thread;
            notify_video();
        } else {
            // impossible !
            dmd_log(LOG_INFO, "in %s, impossible to reach here!\n", __func__);
        }

    } else { // check time elapsed exceeds global.video_duration;

        if (global.client.working_mode == CAPTURE_VIDEO
                || global.client.working_mode == CAPTURE_ALL) {

            // if video capturing is on, continue encode video;
            if (video_capturing_switch == VIDEO_CAPTURING_ON) {
                dmd_log(LOG_INFO, "in function %s, notify video thread\n",
                        __func__);

                // then copy image to buffer, remember to lock it first;
                pthread_rwlock_wrlock(
                        &global.client.thread_attr.bufferYUYV_rwlock);
                memcpy(global.client.bufferingYUYV422, yuyv, length);
                pthread_rwlock_unlock(
                        &global.client.thread_attr.bufferYUYV_rwlock);

                // only notify video thread;
                notify_video();
            }

            // switch off when time elasped exceeds global.video_duration;
            // which also means current motion stoped;
            if ((now - global.client.lasttime >= global.client.video_duration)
                    && (video_capturing_switch == VIDEO_CAPTURING_ON)) {
                dmd_log(LOG_INFO, "in function %s, switch video capture off\n",
                        __func__);

                video_capturing_switch = VIDEO_CAPTURING_OFF;
                
                pthread_mutex_lock(&global_stats->mutex);
                if (global_stats->current_motion != NULL) {
                    // five things to do:
                    // 1. set motion end time
                    // 2. set motion duration;
                    // 3. add global_stats->current_motion to
                    //    global_stats->motion_list
                    // 4. store global_stats->current to database;
                    // 5. set global_stats->current_motion to NULL;
                    dmd_log(LOG_INFO,
                            "set global_stats->current_motion to NULL\n");
                    set_motion_end_time(global_stats->current_motion, now);
                    set_motion_duration(global_stats->current_motion);
                    add_motion(global_stats, global_stats->current_motion);
                    insert_item(opendmd_db, DEFAULT_TABLE,
                            global_stats->current_motion);
                    global_stats->current_motion = NULL;
                }
                pthread_mutex_unlock(&global_stats->mutex);
            }



        } else { // for only capturing picture the current motion is stoped;
#if defined(DEBUG)
            assert(global.client.working_mode == CAPTURE_PICTURE);
#endif
            pthread_mutex_lock(&global_stats->mutex);
            if (global_stats->current_motion != NULL) {
                // TODO: there is statistics unaccuracy here,
                //       may be we should not counting picture data here!

                // five things to do:
                // 1. set motion end time
                // 2. set motion duration;
                // 3. add global_stats->current_motion
                //    to global_stats->motion_list
                // 4. store global_stats->current to database;
                // 5. set global_stats->current_motion to NULL;
                dmd_log(LOG_INFO, "in function %s, line %d,"
                        " set global_stats->current_motion to NULL\n",
                        __func__, __LINE__);
                set_motion_end_time(global_stats->current_motion, now);
                set_motion_duration(global_stats->current_motion);
                add_motion(global_stats, global_stats->current_motion);
                // insert_item(opendmd_db, DEFAULT_TABLE,
                //         global_stats->current_motion);
                global_stats->current_motion = NULL;
            }
            pthread_mutex_unlock(&global_stats->mutex);
        }
    }

    return 0;
}

int read_frame(int fd, struct mmap_buffer *buffers, int width, int height)
{
    int ret = 0;
    struct v4l2_buffer buf;
    bzero(&buf, sizeof(struct v4l2_buffer));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if ((ret = ioctl(fd, VIDIOC_DQBUF, &buf)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_DQBUF error:%s\n", strerror(errno));
        return ret;
    }

    // read process space's data to a file
    assert(buffers[buf.index].length == width * height * 2);
    process_image(buffers[buf.index].start,
        buffers[buf.index].length, width, height);

    // put buf back to queue
    if ((ret = ioctl(fd, VIDIOC_QBUF, &buf)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_QBUF error:%s\n", strerror(errno));
        return ret;
    }

    return ret;
}


int dmd_image_capture(struct v4l2_device_info *v4l2_info)
{
    int fd = v4l2_info->video_device_fd;
    int width = v4l2_info->width;
    int height = v4l2_info->height;
    struct mmap_buffer *buffers = v4l2_info->buffers;

    // client_running is global switchoff changed when 
    // Ctrl + C signal generated;
    while (client_running == 1) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // timeout
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        // TODO: replace select with epoll invoking;
        r = select(fd + 1, &fds, NULL, NULL, &tv);
        if ( r == -1) {
            if (errno == EINTR)
                continue;
            dmd_log(LOG_ERR, "Multi I/O select failed.\n");
            return -1;
        } else if (r == 0) {
            dmd_log(LOG_ERR, "Multi I/O select timeout.\n");
            return -1;
        }

        if (read_frame(fd, buffers, width, height) == -1)
            break;

    }
    
    dmd_log(LOG_INFO, "in %s, main thread exit\n", __func__);

    // when exiting in main thread, remember to record the last motion info;
    pthread_mutex_lock(&global_stats->mutex);
    if (global_stats->current_motion != NULL) {
        time_t now;
        now = time(&now);
        assert(now != -1);
        // five things to do:
        // 1. set motion end time
        // 2. set motion duration;
        // 3. add global_stats->current_motion
        //    to global_stats->motion_list
        // 4. store global_stats->current to database;
        // 5. set global_stats->current_motion to NULL;
        dmd_log(LOG_INFO, "in function %s, line %d,"
                " set global_stats->current_motion to NULL\n",
                __func__, __LINE__);
        set_motion_end_time(global_stats->current_motion, now);
        set_motion_duration(global_stats->current_motion);
        add_motion(global_stats, global_stats->current_motion);
        insert_item(opendmd_db, DEFAULT_TABLE, global_stats->current_motion);
        global_stats->current_motion = NULL;
    }
    pthread_mutex_unlock(&global_stats->mutex);

    while (total_thread != 0) {
        /*
         * video thread is keep in pace with picture thread,
         * so when Ctrl + C signal invoked, video thread may not received the
         * signal; then we notify video thread again at here!
         */
        if (global.client.working_mode == CAPTURE_ALL) {
            pthread_mutex_lock(&global.client.thread_attr.video_mutex);
            global.client.video_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.video_cond);
            pthread_mutex_unlock(&global.client.thread_attr.video_mutex);

            pthread_mutex_lock(&global.client.thread_attr.picture_mutex);
            global.client.picture_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.picture_cond);
            pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);

        } else if (global.client.working_mode == CAPTURE_VIDEO) {
            pthread_mutex_lock(&global.client.thread_attr.video_mutex);
            global.client.video_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.video_cond);
            pthread_mutex_unlock(&global.client.thread_attr.video_mutex);

        } else if (global.client.working_mode == CAPTURE_PICTURE) {
            pthread_mutex_lock(&global.client.thread_attr.picture_mutex);
            global.client.picture_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.picture_cond);
            pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);
        }
    }

    return 0;
}
