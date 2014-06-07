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

int video_flag = 0;

static void siguser2_handler(int sig)
{
    assert(sig == SIGUSR2);
    // dmd_log(LOG_INFO, "catched SIGUSR2!\n");
    video_flag = 1;
}

void *video_thread(void *arg)
{
    int ret = -1;
    static char *h264_filepath;

    signal(SIGUSR2, siguser2_handler);
    for (;;) {
        if (video_flag == 1) {
            
            if (video_capturing_switch == VIDEO_CAPTURING_OFF) {
                // switch on video capturing state and refresh h264 filename;
                video_capturing_switch = VIDEO_CAPTURING_ON;
                h264_filepath = get_h264_filepath();
                assert(h264_filepath != NULL);
            }

            int width = global.image_width;
            int height = global.image_height;
            int length = width * height * 2;

            unsigned char *yuv420p = (unsigned char *)malloc(
                width * height * 1.5 * sizeof(unsigned char));
            assert(yuv420p != NULL);
            bzero(yuv420p, width * height * 1.5 * sizeof(unsigned char));

            unsigned char *yuyv422 = (unsigned char *)malloc(
                width * height * 2 * sizeof(unsigned char));
            assert(yuyv422 != NULL);

            pthread_mutex_lock(&global.yuyv422_lock);
            memcpy(yuyv422, global.bufferingYUYV422, width * height * 2);
            pthread_mutex_unlock(&global.yuyv422_lock);

            // convert Packed YUV422 to Planar YUV420P
            YUYV422toYUV420P((unsigned char *)yuyv422, width, height,
                     yuv420p, length);

            // and encode Planar YUV420P frame to H264 format, using libx264
            dmd_log(LOG_INFO, "encode a frame to %s\n", h264_filepath);
            ret = encode_yuv420p(yuv420p, width, height, h264_filepath);
            assert(ret == 0);

            free(yuv420p);
            yuv420p = NULL;
            free(yuyv422);
            yuyv422 = NULL;
            
            // remember to reset flag;
            video_flag = 0;
        }

    }

    pthread_exit(NULL);
}
