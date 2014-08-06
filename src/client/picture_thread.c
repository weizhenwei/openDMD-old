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
 * File: picture_thread.c
 *
 * Brief: picture capture and save thread;
 *
 * Date: 2014.06.06
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "picture_thread.h"

void *picture_thread(void *arg)
{
    int ret = -1;
    struct path_t *jpeg_filepath = NULL;
    enum main_notify_target notify = NOTIFY_NONE;

    for (;;) {

        pthread_mutex_lock(&global.client.thread_attr.picture_mutex);

        while (global.client.picture_target == NOTIFY_NONE) {
            pthread_cond_wait(&global.client.thread_attr.picture_cond,
                    &global.client.thread_attr.picture_mutex);
        }

        notify = global.client.picture_target;
        global.client.picture_target = NOTIFY_NONE;

        if (notify == NOTIFY_PICTURE) {
            
            int width = global.client.image_width;
            int height = global.client.image_height;
            int length = width * height * 2;

            pthread_rwlock_rdlock(&global.client.thread_attr.bufferYUYV_rwlock);
            memcpy(global.client.pyuyv422buffer,
                    global.client.bufferingYUYV422, length);
            pthread_rwlock_unlock(&global.client.thread_attr.bufferYUYV_rwlock);

            // convert YUYV422 to RGB888
            YUYV422toRGB888INT(global.client.pyuyv422buffer, width, height,
                    global.client.rgbbuffer, length);

            // write to jpeg file;
            jpeg_filepath = client_get_filepath(JPEG_FILE);
            assert(jpeg_filepath != NULL);
            dmd_log(LOG_INFO, "in %s, write a jpegfile to %s\n",
                    __func__, jpeg_filepath->path);
            ret = write_jpeg(jpeg_filepath->path, global.client.rgbbuffer, 100,
                    width, height, 0);
            assert(ret == 0);

            // increase statistics data;
            pthread_mutex_lock(&global_stats->mutex);
#if defined(DEBUG)
            assert(global_stats->current_motion != NULL);
#endif
            increase_motion_pictures(global_stats->current_motion);
            pthread_mutex_unlock(&global_stats->mutex);

            free(jpeg_filepath->path);
            jpeg_filepath->path = NULL;
            free(jpeg_filepath);
            jpeg_filepath = NULL;

            notify = NOTIFY_NONE;
            pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);
        } else if (notify == NOTIFY_EXIT) {
            // signal or main thread told us exit;
            dmd_log(LOG_INFO, "in %s, thread to exit\n", __func__);

            notify = NOTIFY_NONE;
            // remember to unlock the picture_mutex;
            pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);

            break;
        }
    } // for

    pthread_mutex_lock(&total_thread_mutex);
    total_thread--;
    pthread_mutex_unlock(&total_thread_mutex);

    pthread_exit(NULL);
}

int write_jpeg(char *filename, unsigned char *buf, int quality,
    int width, int height, int gray)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *fp;
    int i = 0;
    unsigned char *line;
    int line_length;

    assert(filename != NULL);
    // dmd_log(LOG_INFO, "in function %s, jpeg filename is: %s\n",
    //         __func__, filename);
    if ((fp = fopen(filename, "wb+")) == NULL) {
        dmd_log(LOG_ERR, "fopen error:%s\n", strerror(errno));
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = gray ? 1 : 3;
    cinfo.in_color_space = gray ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    line_length = gray ? width : width * 3;

    line = buf;
    for (i = 0; i < height; i++) {
        jpeg_write_scanlines(&cinfo, &line, 1);
        line += line_length;
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fclose(fp);

    return 0;
}

