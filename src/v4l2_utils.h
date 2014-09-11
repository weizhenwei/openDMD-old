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
 * File: v4l2_utils.h
 *
 * Brief: Wrapper functions about v4l2 api originate from <linux/videodev2.h>
 *
 * Date: 2014.05.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef SRC_V4L2_UTILS_H_
#define SRC_V4L2_UTILS_H_

#include <linux/videodev2.h>

struct mmap_buffer {
    void *start;
    unsigned int length;
};

struct v4l2_device_info {
    int video_device_fd;                     // video device fd;
    const char *video_device_path;           // video device path;
    struct v4l2_capability cap;              // video device capabilities;
    struct v4l2_input input;                 // video input;
    struct v4l2_fmtdesc fmtdesc;             // video format enumeration;
    struct v4l2_format format;               // video stream data format;
    struct v4l2_requestbuffers reqbuffers;   // memory mapping buffers;
    struct v4l2_buffer buffer;               // video buffer;

    int reqbuffer_count;                     // req.count;
    struct mmap_buffer *buffers;             // mmap buffers;

    int width;                               // picture width;
    int height;                              // picture height;
};


// query video device's capability
extern int video_capability(struct v4l2_device_info *v4l2_info);

// set and query video device's input
extern int video_input(struct v4l2_device_info *v4l2_info);

// query video format this video device support
extern int video_fmtdesc(struct v4l2_device_info *v4l2_info);

// set video stream data format
extern int video_setfmt(struct v4l2_device_info *v4l2_info);

// query video stream data format
extern int video_getfmt(struct v4l2_device_info *v4l2_info);

// query video stream data format
extern int video_getfmt(struct v4l2_device_info *v4l2_info);

// set stream fps;
extern int video_set_fps(struct v4l2_device_info *v4l2_info);

// malloc request buffer and mmap it
extern int video_mmap(struct v4l2_device_info *v4l2_info);

#endif  // SRC_V4L2_UTILS_H_
