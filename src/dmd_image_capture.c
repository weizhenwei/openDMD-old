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
 * File: dmd_image_capture.c
 *
 * Brief: capture image from video device. 
 *
 * Date: 2014.05.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "dmd_image_capture.h"

extern unsigned char *referenceYUYV;
int flag = -1;

int write_jpeg(char *filename, unsigned char *buf, int quality,
    int width, int height, int gray)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *fp;
    int i = 0;
    unsigned char *line;
    int line_length;

    if ((fp = fopen(filename, "wb")) == NULL) {
        dmd_log(LOG_ERR, "fopen error.\n");
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

int process_image(void *yuyv, int length, int width, int height)
{
    int ret = -1;
    static int num = 0;

    // at linux/limits.h, #define PATH_MAX 4096
    char image_name[PATH_MAX];

    assert(length > 0);

    unsigned char *rgb = (unsigned char *)malloc(
        width * height * 3 * sizeof(unsigned char));
    assert(rgb);
    if (referenceYUYV == NULL) {
        dmd_log(LOG_INFO, "referenceYUYV == NULL\n");
        flag = 1;
        referenceYUYV = (unsigned char *)malloc(length * sizeof(unsigned char));
        assert(referenceYUYV);
        bzero(referenceYUYV, length * sizeof(unsigned char));
    }

    ret = YUYV422toRGB888INT((unsigned char *)yuyv, width, height, rgb, length);
    if (ret == 0) {
        sprintf(image_name, FILE_NAME, num++);
        ret = write_jpeg(image_name, rgb, 100, width, height, 0);
        assert( ret == 0);
    }

    free(rgb);

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
        dmd_log(LOG_ERR, "ioctl VIDIOC_DQBUF failed.\n");
        return ret;
    }

    // read process space's data to a file
    process_image(buffers[buf.index].start,
        buffers[buf.index].length, width, height);

    // put buf back to queue
    if ((ret = ioctl(fd, VIDIOC_QBUF, &buf)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_QBUF failed.\n");
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
    
    while (1) {
        for (;;) {
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

            if (read_frame(fd, buffers, width, height) == 0)
                break;
        }
    } // while

    return 0;
}
