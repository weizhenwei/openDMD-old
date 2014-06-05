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

// switch on-off for controlling video capturing status;
enum video_capturing_type {
    VIDEO_CAPTURING_ON = 1,
    VIDEO_CAPTURING_OFF = 2,
};

static enum video_capturing_type video_capturing_switch = VIDEO_CAPTURING_OFF;

static int test_and_create(const char *path)
{
    if (access(path, F_OK) == -1) { // if path didn't exist, mkdir it;
        if (mkdir(path, 0755) == -1) {
            dmd_log(LOG_ERR, "mkdir %s error:%s\n", path, strerror(errno));
            return -1;
        }
    } else { // path exists, be sure it is directory;
        struct stat path_stat;
        if (stat(path, &path_stat) == -1) {
            dmd_log(LOG_ERR, "stat error:%s\n", path, strerror(errno));
            return -1;
        }
        if (!S_ISDIR(path_stat.st_mode)) { // first delete, then mkdir;
            if (unlink(path) == -1) {
                dmd_log(LOG_ERR, "unlink error:%s", strerror(errno));
                return -1;
            }
            if (mkdir(path, 0755) == -1) {
                dmd_log(LOG_ERR, "mkdir %s error:%s\n", path, strerror(errno));
                return -1;
            }
        }
    } // else

    return 0;
}

char *get_jpeg_filepath()
{
    time_t now;
    struct tm *tmptr;
    // at linux/limits.h, #define PATH_MAX 4096
    static char filepath[PATH_MAX];
    char storepath[PATH_MAX];
    strncpy(storepath, global.store_dir, strlen(global.store_dir));
    storepath[strlen(global.store_dir)] = '\0';
    strcat(storepath, "/jpeg");
    assert(test_and_create(storepath) == 0);

    now = time(&now);
    assert(now != -1);

    tmptr = localtime(&now);
    assert(tmptr != NULL);
    sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d-%02d.jpg",
            storepath, 
            tmptr->tm_year + 1900,
            tmptr->tm_mon + 1,
            tmptr->tm_mday,
            tmptr->tm_hour,
            tmptr->tm_min,
            tmptr->tm_sec,
            global.counter_in_second);

    assert(strlen(filepath) < PATH_MAX);

    dmd_log(LOG_INFO, "in function %s, jpeg filename is: %s\n",
            __func__, filepath);

    return filepath;
}

char *get_h264_filepath()
{
    time_t now;
    struct tm *tmptr;
    // at linux/limits.h, #define PATH_MAX 4096
    static char filepath[PATH_MAX];
    char storepath[PATH_MAX];
    strncpy(storepath, global.store_dir, strlen(global.store_dir));
    storepath[strlen(global.store_dir)] = '\0';
    strcat(storepath, "/h264");
    assert(test_and_create(storepath) == 0);

    now = time(&now);
    assert(now != -1);

    tmptr = localtime(&now);
    assert(tmptr != NULL);
    sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d.h264",
            storepath,
            tmptr->tm_year + 1900,
            tmptr->tm_mon + 1,
            tmptr->tm_mday,
            tmptr->tm_hour,
            tmptr->tm_min,
            tmptr->tm_sec);
    assert(strlen(filepath) < PATH_MAX);

    dmd_log(LOG_INFO, "in function %s, h264 filename is: %s\n",
            __func__, filepath);

    return filepath;
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
    if ((fp = fopen(filename, "wb")) == NULL) {
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

int process_image(void *yuyv, int length, int width, int height)
{
    int ret = -1;
    char *jpeg_filepath = NULL;

    // may use more than once before refreshed, so make it static;
    static char *h264_filepath = NULL;
    time_t now;

    now = time(&now);
    assert(now != -1);

    assert(length == width * height * 2);

    unsigned char *rgb = (unsigned char *)malloc(
        width * height * 3 * sizeof(unsigned char));
    assert(rgb != NULL);

    // convert YUYV422 to RGB888
    ret = YUYV422toRGB888INT((unsigned char *)yuyv, width, height,
            rgb, length);

    if (ret == 0) { // a motion detected!

        // refresh the time last motion detected;
        if (global.lasttime == now) {
            global.counter_in_second++;
        } else {
            global.counter_in_second = 0;
        }
        global.lasttime = now;

        // optional capture picture;
        if (global.working_mode == CAPTURE_PICTURE
                || global.working_mode == CAPTURE_ALL) {
            jpeg_filepath = get_jpeg_filepath();
            ret = write_jpeg(jpeg_filepath, rgb, 100, width, height, 0);
            assert(ret == 0);
            free(rgb);
            // Warning: rgb = NULL is necessary.
            rgb = NULL;
        }

        // optional capture video;
        if (global.working_mode == CAPTURE_VIDEO
                || global.working_mode == CAPTURE_ALL) {
            if (video_capturing_switch == VIDEO_CAPTURING_OFF) {
                // switch on video capturing state and refresh h264 filename;
                video_capturing_switch = VIDEO_CAPTURING_ON;
                h264_filepath = get_h264_filepath();
                assert(h264_filepath != NULL);
            }

            // convert Packed YUV422 to Planar YUV420P
            unsigned char *yuv420p = (unsigned char *)malloc(
                width * height * 1.5 * sizeof(unsigned char));
            assert(yuv420p != NULL);
            bzero(yuv420p, width * height * 1.5 * sizeof(unsigned char));
            ret = YUYV422toYUV420P((unsigned char *)yuyv, width, height,
                    yuv420p, length);

            // and encode Planar YUV420P frame to H264 format, using libx264
            ret = encode_yuv420p(yuv420p, width, height, h264_filepath);
            assert(ret == 0);
            free(yuv420p);
        }

    } else { // check time elapsed exceeds global.video_duration;

        if (global.working_mode == CAPTURE_VIDEO
                || global.working_mode == CAPTURE_ALL) {

            // if video capturing is on, continue encode video;
            if (video_capturing_switch == VIDEO_CAPTURING_ON) {
                // convert Packed YUV422 to Planar YUV420P
                unsigned char *yuv420p = (unsigned char *)malloc(
                    width * height * 1.5 * sizeof(unsigned char));
                assert(yuv420p != NULL);
                bzero(yuv420p, width * height * 1.5 * sizeof(unsigned char));
                ret = YUYV422toYUV420P((unsigned char *)yuyv, width, height,
                        yuv420p, length);

                // encode Planar YUV420P frame to H264 format using libx264
                ret = encode_yuv420p(yuv420p, width, height, h264_filepath);
                assert(ret == 0);
                free(yuv420p);
            }

            // switch off when time elasped exceeds global.video_duration;
            if (now - global.lasttime >= global.video_duration) {
                video_capturing_switch = VIDEO_CAPTURING_OFF;
            }
        }

    }

    // if no motion occured, or capture video only,
    // rgb is not released at this moment, so free it;
    // TODO: after free a pointer, why it is till NON-NULL?
    if (rgb != NULL) {
        free(rgb);
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
