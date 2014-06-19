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
 * File: video_thread.c
 *
 * Brief: video capture and save thread;
 *
 * Date: 2014.06.06
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "video_thread.h"

enum video_capturing_type video_capturing_switch = VIDEO_CAPTURING_OFF;

void *video_thread(void *arg)
{
    int ret = -1;
    static char *h264_filepath;
    enum main_notify_target notify = NOTIFY_NONE;

    for (;;) {

        pthread_mutex_lock(&global.client.thread_attr.video_mutex);

        while (global.client.video_target == NOTIFY_NONE) {
            pthread_cond_wait(&global.client.thread_attr.video_cond,
                    &global.client.thread_attr.video_mutex);
        }
        notify = global.client.video_target;
        global.client.video_target = NOTIFY_NONE;

        if (notify == NOTIFY_VIDEO) {

            if (video_capturing_switch == VIDEO_CAPTURING_OFF) {
                // switch on video capturing state and refresh h264 filename;
                video_capturing_switch = VIDEO_CAPTURING_ON;
                h264_filepath = get_h264_filepath();
                assert(h264_filepath != NULL);
            }

            int width = global.client.image_width;
            int height = global.client.image_height;
            int length = width * height * 2;

            pthread_rwlock_rdlock(&global.client.thread_attr.bufferYUYV_rwlock);
            memcpy(global.client.vyuyv422buffer,
                    global.client.bufferingYUYV422, length);
            pthread_rwlock_unlock(&global.client.thread_attr.bufferYUYV_rwlock);

            // convert Packed YUV422 to Planar YUV420P
            YUYV422toYUV420P(global.client.vyuyv422buffer, width, height,
                    global.client.yuv420pbuffer, length);

            // and encode Planar YUV420P frame to H264 format, using libx264
            dmd_log(LOG_INFO, "in %s, encode a frame to %s\n",
                    __func__, h264_filepath);
            ret = encode_yuv420p(global.client.yuv420pbuffer, width, height,
                    h264_filepath);
            assert(ret == 0);

        } else if (notify == NOTIFY_EXIT) {
            // signal or main thread told us exit;
            dmd_log(LOG_INFO, "in %s, thread to exit\n", __func__);
            break;
        }

        notify = NOTIFY_NONE;

        pthread_mutex_unlock(&global.client.thread_attr.video_mutex);
    }

    pthread_exit(NULL);
}
