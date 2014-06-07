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
 * File: video.c
 *
 * Brief: video manipulation interface of the project 
 *
 * Date: 2014.05.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "video.h"

// define global variable dmd_video;
struct v4l2_device_info *dmd_video = NULL;

struct v4l2_device_info *dmd_video_create(const char *device_path)
{
    struct v4l2_device_info *device;
    device =
        (struct v4l2_device_info *)malloc(sizeof(struct v4l2_device_info));
    if (device == NULL) {
        dmd_log(LOG_ERR, "malloc for struct v4l2_device_info failed.\n");
        return device;
    }

    bzero(device, sizeof(device));

    device->video_device_path = device_path;
    device->reqbuffer_count = global.req_count;
    device->buffers =
        malloc(device->reqbuffer_count * sizeof(struct mmap_buffer));
    assert(device->buffers != NULL);

    device->width = global.image_width;
    device->height = global.image_height;

    return device;
}

int dmd_video_open(struct v4l2_device_info *v4l2_info)
{
    int fd = -1;
    const char *devpath = v4l2_info->video_device_path;
    dmd_log(LOG_INFO, "video device:%s\n", devpath);
    if ((fd = open(devpath, O_RDWR)) == -1) {
        dmd_log(LOG_ERR, "open video device error:%s\n", strerror(errno));
        return -1;
    }

    v4l2_info->video_device_fd = fd;

    return 1;
}

int dmd_video_init(struct v4l2_device_info *v4l2_info)
{
    int ret;
    // query video device's capability
    if ((ret = video_capability(v4l2_info)) == -1) {
        return ret;
    }

    // query and set video input format
    if ((ret = video_input(v4l2_info)) == -1) {
        return ret;
    }

    // traverse video stream format
    if ((ret = video_fmtdesc(v4l2_info)) == -1) {
        return ret;
    }

    // set video stream data format
    if ((ret = video_setfmt(v4l2_info)) == -1) {
        return ret;
    }
    // get video stream data format
    if ((ret = video_getfmt(v4l2_info)) == -1) {
        return ret;
    }
    
    // memory map for the request buffer
    if ((ret = video_mmap(v4l2_info)) == -1) {
        return ret;
    }

    return ret;
}

/*
 * enum v4l2_buf_type {
 * 	V4L2_BUF_TYPE_VIDEO_CAPTURE        = 1,
 * 	V4L2_BUF_TYPE_VIDEO_OUTPUT         = 2,
 * 	V4L2_BUF_TYPE_VIDEO_OVERLAY        = 3,
 * 	V4L2_BUF_TYPE_VBI_CAPTURE          = 4,
 * 	V4L2_BUF_TYPE_VBI_OUTPUT           = 5,
 * 	V4L2_BUF_TYPE_SLICED_VBI_CAPTURE   = 6,
 * 	V4L2_BUF_TYPE_SLICED_VBI_OUTPUT    = 7,
 * #if 1
 * 	// Experimental
 * 	V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
 * #endif
 * 	V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
 * 	V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE  = 10,
 * 	V4L2_BUF_TYPE_PRIVATE              = 0x80,
 * };
 *
 * open the video stream
 */

int dmd_video_streamon(struct v4l2_device_info *v4l2_info)
{
    int ret = 0, i = 0;
    int n_buffer = v4l2_info->reqbuffer_count;
    int fd = v4l2_info->video_device_fd;

    // place kernel requestbuffers to a  queue
    for (i = 0; i < n_buffer; i++) {
        struct v4l2_buffer buf;
        bzero(&buf, sizeof(struct v4l2_buffer));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        
        // requestbuffer to queue
        if ((ret = ioctl(fd, VIDIOC_QBUF, &buf)) == -1) {
            dmd_log(LOG_ERR, "ioctl VIDIOC_QBUF error:%s\n", strerror(errno));
            return ret;
        }
    }
    
    // start stream on, start capture data
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if ((ret = ioctl(fd, VIDIOC_STREAMON, &type)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_STREAMON error:%s\n", strerror(errno));
        return ret;
    }


    dmd_log(LOG_INFO, "Video stream is now on!\n");

    return ret;
}

int dmd_video_streamoff(struct v4l2_device_info *v4l2_info)
{

    int ret = 0;
    unsigned int i = 0;
    int n_buffer = v4l2_info->reqbuffer_count;
    int fd = v4l2_info->video_device_fd;
    struct mmap_buffer *buffers = v4l2_info->buffers;

    // stop stream off, stop capture
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if ((ret = ioctl(fd, VIDIOC_STREAMOFF, &type)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_STREAMOFF error:%s\n", strerror(errno));
        return ret;
    }

    dmd_log(LOG_INFO, "Video stream is now off!\n");

    // munmap
    for (i = 0; i < n_buffer; i++) {
        if ((ret = munmap(buffers[i].start, buffers[i].length)) == -1) {
            dmd_log(LOG_ERR, "munmap failed:%s\n", strerror(errno));
            return ret;
        }
    }

    return ret;
}

int dmd_video_close(struct v4l2_device_info *v4l2_info)
{
    if (close(v4l2_info->video_device_fd) == -1) {
        dmd_log(LOG_ERR, "close video device error:%s\n", strerror(errno));
        return -1;
    }

    return 1;
}

void dmd_video_release(struct v4l2_device_info *v4l2_info)
{
    if (v4l2_info != NULL) {
        if (v4l2_info->buffers) {
            free(v4l2_info->buffers);
        }

        free(v4l2_info);
    }
}

