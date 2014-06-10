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
 * File: global_context.c
 *
 * Brief: struct global_context is used to control opendmd running.
 *
 * Date: 2014.05.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "global_context.h"

// define the global variable global to control opendmd running;
struct global_context global;

void init_default_global()
{
    // basic global information
    global.pid = getpid();
#if defined(DEBUG)
    global.log_level = LOG_INFO;   // default log level;
#else
    global.log_level = LOG_ERR;   // default log level;
#endif
    global.cluster_mode = CLUSTER_SINGLETON;

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
    strncpy(global.cfg_file, DEFAULT_RELEASE_CFG_FILE,
            strlen(DEFAULT_RELEASE_CFG_FILE));
    global.cfg_file[strlen(DEFAULT_RELEASE_CFG_FILE)] = '\0';
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
    global.lasttime = time(&global.lasttime);
    assert(global.lasttime != -1);
    global.counter_in_second = 0;
    global.video_duration = 5;

    // reusable image buffer is initialized in parse_config(),
    // where after config file parsing, image_width and image_height
    // are finally determined;
    global.referenceYUYV422 = NULL;
    global.rgbbuffer = NULL;
    global.pyuyv422buffer = NULL;
    global.vyuyv422buffer = NULL;
    global.yuv420pbuffer = NULL;
    global.bufferingYUYV422 = NULL;


    // set thread attribute;
    pthread_attr_init(&global.thread_attr.global_attr);
    struct sched_param param;
    param.sched_priority = SCHED_RR;
    pthread_attr_setschedparam(&global.thread_attr.global_attr, &param);
    pthread_attr_setdetachstate(&global.thread_attr.global_attr,
            PTHREAD_CREATE_JOINABLE);

    pthread_rwlock_init(&global.thread_attr.bufferYUYV_rwlock, NULL);
    pthread_mutex_init(&global.thread_attr.picture_mutex, NULL);
    pthread_cond_init(&global.thread_attr.picture_cond, NULL);
    pthread_mutex_init(&global.thread_attr.video_mutex, NULL);
    pthread_cond_init(&global.thread_attr.video_cond, NULL);
    global.thread_attr.picture_thread_id = 0;
    global.thread_attr.video_thread_id = 0;

    global.picture_target = NOTIFY_NONE;
    global.video_target = NOTIFY_NONE;


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
}

void dump_global_config()
{
    // only dump, no error detect.
    dmd_log(LOG_INFO, "in function %s:\n", __func__);

    // basic settings;
    if (global.daemon_mode == DAEMON_ON) {
        dmd_log(LOG_INFO, "daemon_mode: on\n");
    } if (global.daemon_mode == DAEMON_OFF) {
        dmd_log(LOG_INFO, "daemon_mode: off\n");
    }

    if (global.working_mode == CAPTURE_VIDEO) {
        dmd_log(LOG_INFO, "working_mode: capture video\n");
    } else if (global.working_mode == CAPTURE_PICTURE) {
        dmd_log(LOG_INFO, "working_mode: capture picture\n");
    } else if (global.working_mode == CAPTURE_ALL) {
        dmd_log(LOG_INFO, "working_mode: capture video and picture\n");
    }

    dmd_log(LOG_INFO, "pid file:%s\n", global.pid_file);
    dmd_log(LOG_INFO, "cfg file:%s\n", global.cfg_file);

    dmd_log(LOG_INFO, "video device:%s\n", global.video_device);
    dmd_log(LOG_INFO, "image width:%d\n", global.image_width);
    dmd_log(LOG_INFO, "image height:%d\n", global.image_height);
    dmd_log(LOG_INFO, "req count:%d\n", global.req_count);

    dmd_log(LOG_INFO, "diff pixels:%d\n", global.diff_pixels);
    dmd_log(LOG_INFO, "diff deviation:%d\n", global.diff_deviation);
    dmd_log(LOG_INFO, "video duration:%d\n", global.video_duration);

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

// called at atexit() to free malloced memory in variable global;
void release_default_global()
{
    dmd_log(LOG_INFO, "at function %s, free malloced memory\n", __func__);

    // free reusable buffers;
    free(global.referenceYUYV422);
    global.referenceYUYV422 = NULL;

    free(global.rgbbuffer);
    global.rgbbuffer = NULL;

    free(global.pyuyv422buffer);
    global.pyuyv422buffer = NULL;

    free(global.vyuyv422buffer);
    global.vyuyv422buffer = NULL;

    free(global.yuv420pbuffer);
    global.yuv420pbuffer = NULL;

    free(global.bufferingYUYV422);
    global.bufferingYUYV422 = NULL;

    // clean threads;

    pthread_join(global.thread_attr.picture_thread_id, NULL);
    pthread_join(global.thread_attr.video_thread_id, NULL);

    pthread_attr_destroy(&global.thread_attr.global_attr);
    pthread_rwlock_destroy(&global.thread_attr.bufferYUYV_rwlock);
    pthread_mutex_destroy(&global.thread_attr.picture_mutex);
    pthread_cond_destroy(&global.thread_attr.picture_cond);
    pthread_mutex_destroy(&global.thread_attr.video_mutex);
    pthread_cond_destroy(&global.thread_attr.video_cond);
}
