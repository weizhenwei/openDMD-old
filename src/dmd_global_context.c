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
 * File: dmd_global_context.c
 *
 * Brief: struct global_context is used to control opendmd running.
 *
 * Date: 2014.05.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "dmd_global_context.h"

extern struct global_context global;

void init_default_global()
{
    // global running settings;
#if defined(DEBUG)
    global.daemon_mode = DAEMON_OFF;
#else
    global.daemon_mode = DAEMON_ON;
#endif

    global.working_mode = CAPTURE_VIDEO;

#if defined(DEBUG)
    /* Warning:If there is no null byte among the first n bytes of
     * src, the string placed in dest will not be null-terminated,
     * remember add null-terminated manually.
     */
    assert(strlen(DEFAULT_DEBUG_PID_FILE) < PATH_MAX);
    strncpy(global.pid_file, DEFAULT_DEBUG_PID_FILE,
            strlen(DEFAULT_DEBUG_PID_FILE));
    global.pid_file[strlen(DEFAULT_DEBUG_PID_FILE)] = '\0';

    assert(strlen(DEFAULT_DEBUG_CFG_FILE) < PATH_MAX);
    strncpy(global.cfg_file, DEFAULT_DEBUG_CFG_FILE,
            strlen(DEFAULT_DEBUG_CFG_FILE));
    global.cfg_file[strlen(DEFAULT_DEBUG_CFG_FILE)] = '\0';
#else
    assert(strlen(DEFAULT_RELEASE_PID_FILE) < PATH_MAX);
    strncpy(global.pid_file, DEFAULT_RELEASE_PID_FILE,
            strlen(DEFAULT_RELEASE_PID_FILE));
    global.pid_file[strlen(DEFAULT_RELEASE_PID_FILE)] = '\0';

    assert(strlen(DEFAULT_RELEASE_CFG_FILE) < PATH_MAX);
    strncpy(global.pid_file, DEFAULT_RELEASE_CFG_FILE,
            strlen(DEFAULT_RELEASE_CFG_FILE));
    global.pid_file[strlen(DEFAULT_RELEASE_CFG_FILE)] = '\0';
#endif

    // video device settings;
    assert(strlen(DEFAULT_VIDEO_DEVICE) < PATH_MAX);
    strncpy(global.video_device, DEFAULT_VIDEO_DEVICE,
            strlen(DEFAULT_VIDEO_DEVICE));
    global.video_device[strlen(DEFAULT_VIDEO_DEVICE)] = '\0';
    global.image_width = DEFAULT_VIDEO_WIDTH;
    global.image_height = DEFAULT_VIDEO_HEIGHT;
    global.req_count = DEFAULT_REQCOUNT;

    // motion detection threshold settings;
    global.diff_pixels = DEFAULT_DIFF_PIXELS;
    global.diff_deviation = DEFAULT_DIFF_DEVIATION;

    // captured picture/video storage settings;
    global.picture_format = PICTURE_JPEG;
    global.video_format = VIDEO_H264;
#if defined(DEBUG)
    assert(strlen(DEFAULT_DEBUG_STORE_DIR) < PATH_MAX);
    strncpy(global.store_dir, DEFAULT_DEBUG_STORE_DIR,
            strlen(DEFAULT_DEBUG_STORE_DIR));
    global.store_dir[strlen(DEFAULT_DEBUG_STORE_DIR)] = '\0';
#else
    assert(strlen(DEFAULT_RELEASE_STORE_DIR) < PATH_MAX);
    strncpy(global.store_dir, DEFAULT_RELEASE_STORE_DIR,
            strlen(DEFAULT_RELEASE_STORE_DIR));
    global.store_dir[strlen(DEFAULT_RELEASE_STORE_DIR)] = '\0';
#endif

    dmd_log(LOG_INFO, "default pid file:%s\n", global.pid_file);
    dmd_log(LOG_INFO, "default cfg file:%s\n", global.cfg_file);
    dmd_log(LOG_INFO, "default store file:%s\n", global.store_dir);
}

void dump_global_config()
{
    // only dump, no error detect.
    dmd_log(LOG_INFO, "in %s:\n", __func__);

    // basic settings;
    if (global.daemon_mode == DAEMON_ON) {
        dmd_log(LOG_INFO, "daemon_mode: on\n");
    } if (global.daemon_mode == DAEMON_OFF) {
        dmd_log(LOG_INFO, "daemon_mode: off\n");
    }

    if (global.working_mode == CAPTURE_VIDEO) {
        dmd_log(LOG_INFO, "working_mode: cpature video\n");
    } else if (global.working_mode == CAPTURE_PICTURE) {
        dmd_log(LOG_INFO, "working_mode: cpature picture\n");
    } else if (global.working_mode == CAPTURE_ALL) {
        dmd_log(LOG_INFO, "working_mode: cpature video and picture\n");
    }

    dmd_log(LOG_INFO, "pid file:%s\n", global.pid_file);
    dmd_log(LOG_INFO, "cfg file:%s\n", global.cfg_file);

    dmd_log(LOG_INFO, "video device:%s\n", global.video_device);
    dmd_log(LOG_INFO, "image width:%d\n", global.image_width);
    dmd_log(LOG_INFO, "image height:%d\n", global.image_height);
    dmd_log(LOG_INFO, "req count:%d\n", global.req_count);

    dmd_log(LOG_INFO, "diff pixels:%d\n", global.diff_pixels);
    dmd_log(LOG_INFO, "diff deviation:%d\n", global.diff_deviation);

    if (global.picture_format == PICTURE_BMP) {
        dmd_log(LOG_INFO, "picture_format: bmp\n");
    } else if (global.picture_format == PICTURE_PNG) {
        dmd_log(LOG_INFO, "picture_format: png\n");
    } else if (global.picture_format == PICTURE_JPEG) {
        dmd_log(LOG_INFO, "picture_format: jpeg\n");
    }

    if (global.video_format == VIDEO_H264) {
        dmd_log(LOG_INFO, "video_format: h264\n");
    }

    dmd_log(LOG_INFO, "store dir:%s\n", global.store_dir);
}
