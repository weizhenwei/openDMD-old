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

                // then copy image to buffer, remember to lock it first;
                pthread_rwlock_wrlock(&global.client.thread_attr.bufferYUYV_rwlock);
                memcpy(global.client.bufferingYUYV422, yuyv, length);
                pthread_rwlock_unlock(&global.client.thread_attr.bufferYUYV_rwlock);

                // only notify video thread;
                notify_video();
            }

            // switch off when time elasped exceeds global.video_duration;
            if ((now - global.client.lasttime >= global.client.video_duration)
                    && (video_capturing_switch == VIDEO_CAPTURING_ON)) {
                dmd_log(LOG_INFO, "switch video capture off\n");
                video_capturing_switch = VIDEO_CAPTURING_OFF;
            }
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

    while (client_running == 1) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // timeout
        tv.tv_sec = 2;
        tv.tv_usec = 0;

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

    return 0;
}
