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
 * File: dmd_global_context.h
 *
 * Brief: struct global_context is used to control opendmd running.
 *
 * Date: 2014.05.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef DMD_GLOBAL_CONTEXT_H
#define DMD_GLOBAL_CONTEXT_H

struct global_context global;
struct global_context default_context;

// opendmd run in daemon mode;
enum daemon_mode_type {
    DAEMON_ON = 1,
    DAEMON_OFF = 2,
};

// the opendmd working mode: capture picture, capture video or capture all;
enum working_type {
    CAPTURE_PICTURE = 1,
    CAPTURE_VIDEO = 2,
    CPATURE_ALL = 3,
};

// captured picture format, valid value: bmp, jpeg, png;
// at present support jpeg only.
enum picture_format_type {
    PICTURE_BMP = 1,
    PICTURE_PNG = 2,
    PICTURE_JPEG = 3,
};

// captured picture format, valid value: h264
// at present support h264 only.
enum video_format_type {
    VIDEO_H264 = 1,
};

struct global_context {
    // global running settings
    enum daemon_mode_type daemon_mode; // run in daemon mode;
    enum working_type  working_mode;   // working mode: picture, video or all;
    char *pid_file;                    // main process's pid file;

    // video device settings;
    char *video_device;        // video device path;
    unsigned int image_width;  // image width in pixels; 
    unsigned int image_height; // image height in pixels; 
    unsigned int req_count;    // mmap req.count;

    // motion detection threshold settings;
    unsigned int diff_pixels;    // the pixels threshold motion occured.
    unsigned int diff_deviation; // the deviation pixel allowed.

    // captured pictures/video storage settings;
    enum picture_format_type picture_format; // captured picture format.
    enum video_format_type video_format;     // captured video format;
    char *store_dir;                         // captured pictures/video
                                             // storage directory.
};

#endif
