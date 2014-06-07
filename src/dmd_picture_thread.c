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
 * File: dmd_picture_thread.c
 *
 * Brief: picture capture and save thread;
 *
 * Date: 2014.06.06
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "dmd_picture_thread.h"

int picture_flag = 0;

static void siguser1_handler(int sig)
{
    assert(sig == SIGUSR1);
    // dmd_log(LOG_INFO, "catched SIGUSR1!\n");
    picture_flag = 1;
}

void *picture_thread(void *arg)
{
    int ret = -1;
    char *jpeg_filepath = NULL;

    signal(SIGUSR1, siguser1_handler);
    for (;;) {
        if (picture_flag == 1) {
            
            int width = global.image_width;
            int height = global.image_height;
            int length = width * height * 2;

            unsigned char *rgb = (unsigned char *)malloc(
                width * height * 3 * sizeof(unsigned char));
            assert(rgb != NULL);
            bzero(rgb, width * height * 3 * sizeof(unsigned char));

            unsigned char *yuyv422 = (unsigned char *)malloc(
                width * height * 2 * sizeof(unsigned char));
            assert(yuyv422 != NULL);

            pthread_mutex_lock(&global.yuyv422_lock);
            memcpy(yuyv422, global.bufferingYUYV422, width * height * 2);
            pthread_mutex_unlock(&global.yuyv422_lock);

            // convert YUYV422 to RGB888
            YUYV422toRGB888INT((unsigned char *)yuyv422, width, height,
                    rgb, length);

            // write to jpeg file;
            jpeg_filepath = get_jpeg_filepath();
            ret = write_jpeg(jpeg_filepath, rgb, 100, width, height, 0);
            assert(ret == 0);

            free(rgb);
            rgb = NULL;
            free(yuyv422);
            yuyv422 = NULL;
            
            // remember to reset flag!
            picture_flag = 0;
        }

    }

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
    dmd_log(LOG_INFO, "in function %s, jpeg filename is: %s\n",
            __func__, filename);
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

