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


// for thread synchronization;
unsigned int total_thread = 0;
pthread_mutex_t total_thread_mutex = PTHREAD_MUTEX_INITIALIZER;

// define the global variable global to control opendmd running;
struct global_context global;

static void init_client_rtp()
{
    global.client.clientrtp.rtpsession = NULL;
    global.client.clientrtp.user_ts = 0;

    assert(strlen(LOCAL_IP) < PATH_MAX);
    strncpy(global.client.clientrtp.local_ip, LOCAL_IP, strlen(LOCAL_IP));
    global.client.clientrtp.local_ip[strlen(LOCAL_IP)] = '\0';
    global.client.clientrtp.local_port = LOCAL_PORT;
    global.client.clientrtp.local_sequence_number = LOCAL_SEQUENCE_NUMBER;

    assert(strlen(SERVER_IP) < PATH_MAX);
    strncpy(global.client.clientrtp.server_ip, SERVER_IP, strlen(SERVER_IP));
    global.client.clientrtp.server_ip[strlen(SERVER_IP)] = '\0';
    global.client.clientrtp.server_port_base = SERVER_PORT_BASE;
    global.client.clientrtp.server_rtp_port =
        global.client.clientrtp.server_port_base +
        2 * global.client.clientrtp.local_sequence_number;
    global.client.clientrtp.server_rtcp_port =
        global.client.clientrtp.server_port_base +
        2 * global.client.clientrtp.local_sequence_number + 1;
}

static void init_default_client()
{
    // video device settings;
    assert(strlen(DEFAULT_VIDEO_DEVICE) < PATH_MAX);
    strncpy(global.client.video_device, DEFAULT_VIDEO_DEVICE,
            strlen(DEFAULT_VIDEO_DEVICE));
    global.client.video_device[strlen(DEFAULT_VIDEO_DEVICE)] = '\0';
    global.client.image_width = DEFAULT_VIDEO_WIDTH;
    global.client.image_height = DEFAULT_VIDEO_HEIGHT;
    global.client.req_count = DEFAULT_REQCOUNT;

    // motion detection threshold settings;
    global.client.diff_pixels = DEFAULT_DIFF_PIXELS;
    global.client.diff_deviation = DEFAULT_DIFF_DEVIATION;
    global.client.lasttime = time(&global.client.lasttime);
    assert(global.client.lasttime != -1);
    global.client.counter_in_second = 0;
    global.client.video_duration = 5;

    // reusable image buffer is initialized in parse_config(),
    // where after config file parsing, image_width and image_height
    // are finally determined;
    global.client.referenceYUYV422 = NULL;
    global.client.rgbbuffer = NULL;
    global.client.pyuyv422buffer = NULL;
    global.client.vyuyv422buffer = NULL;
    global.client.yuv420pbuffer = NULL;
    global.client.bufferingYUYV422 = NULL;


    // FIXME: according config item:working mode to initialize picture thread
    //        or video thread or both.

    // set thread attribute;
    pthread_attr_init(&global.client.thread_attr.global_attr);
    struct sched_param param;
    param.sched_priority = SCHED_RR;
    pthread_attr_setschedparam(&global.client.thread_attr.global_attr, &param);
    pthread_attr_setdetachstate(&global.client.thread_attr.global_attr,
            PTHREAD_CREATE_JOINABLE);

    pthread_rwlock_init(&global.client.thread_attr.bufferYUYV_rwlock, NULL);
    pthread_mutex_init(&global.client.thread_attr.picture_mutex, NULL);
    pthread_cond_init(&global.client.thread_attr.picture_cond, NULL);
    pthread_mutex_init(&global.client.thread_attr.video_mutex, NULL);
    pthread_cond_init(&global.client.thread_attr.video_cond, NULL);
    global.client.thread_attr.picture_thread_id = 0;
    global.client.thread_attr.video_thread_id = 0;

    global.client.picture_target = NOTIFY_NONE;
    global.client.video_target = NOTIFY_NONE;


    // captured picture/video storage settings;
    global.client.picture_format = PICTURE_JPEG;
    global.client.video_format = VIDEO_H264;

    // TODO: need more optimization;
    const char *home = getenv("HOME");
    sprintf(global.client.client_repo, "%s/opendmd/client_repo", home);
#if 0
#if defined(DEBUG)
    assert(strlen(DEFAULT_DEBUG_STORE_DIR) < PATH_MAX);
    strncpy(global.client.client_repo, DEFAULT_DEBUG_STORE_DIR,
            strlen(DEFAULT_DEBUG_STORE_DIR));
    global.client.client_repo[strlen(DEFAULT_DEBUG_STORE_DIR)] = '\0';
#else
    assert(strlen(DEFAULT_RELEASE_STORE_DIR) < PATH_MAX);
    strncpy(global.client.client_repo, DEFAULT_RELEASE_STORE_DIR,
            strlen(DEFAULT_RELEASE_STORE_DIR));
    global.client.client_repo[strlen(DEFAULT_RELEASE_STORE_DIR)] = '\0';
#endif
#endif

    // initialize rtpsession associated;
    init_client_rtp();
}

static void init_default_server()
{
    // TODO: need more optimization;
    const char *home = getenv("HOME");
    sprintf(global.server.server_repo, "%s/opendmd/server_repo", home);
#if 0
    assert(strlen(SERVER_REPO) < PATH_MAX);
    strncpy(global.server.server_repo, SERVER_REPO,
            strlen(SERVER_REPO));
    global.server.server_repo[strlen(SERVER_REPO)] = '\0';
#endif

    global.server.user_ts = 0;
    global.server.client_scale = CLIENT_SCALE;
    // for server ortp ip/port;
    assert(strlen(SERVER_IP) < PATH_MAX);
    strncpy(global.server.server_ip, SERVER_IP, strlen(SERVER_IP));
    global.server.server_ip[strlen(SERVER_IP)] = '\0';
    global.server.last_duration = 0;
    global.server.client_items = NULL;   // this will be malloced
                                         // after config parsing;
}

void init_default_global()
{
    // basic global information

    global.cluster_mode = CLUSTER_SINGLETON;

    global.pid = getpid();
#if defined(DEBUG)
    global.log_level = LOG_INFO;   // default log level;
#else
    global.log_level = LOG_ERR;   // default log level;
#endif

    // global running settings;
#if defined(DEBUG)
    global.daemon_mode = DAEMON_OFF;
#else
    global.daemon_mode = DAEMON_ON;
#endif

    global.client.working_mode = CAPTURE_VIDEO;

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

    global.x264_fps = DEFAULT_X264_FPS;

    // init client/server specific;
    init_default_client();
    init_default_server();
}

static void dump_client_rtp()
{
    dmd_log(LOG_INFO, "local ip:%s\n", global.client.clientrtp.local_ip);
    dmd_log(LOG_INFO, "local port:%d\n",
            global.client.clientrtp.local_port);
    dmd_log(LOG_INFO, "local_sequence_number:%d\n",
            global.client.clientrtp.local_sequence_number);

    dmd_log(LOG_INFO, "server ip:%s\n",
            global.client.clientrtp.server_ip);
    dmd_log(LOG_INFO, "server port base %d\n",
            global.client.clientrtp.server_port_base);
    dmd_log(LOG_INFO, "server rtp port:%d\n",
            global.client.clientrtp.server_rtp_port);
    dmd_log(LOG_INFO, "server rtcp port:%d\n",
            global.client.clientrtp.server_rtcp_port);

}

static int dump_client()
{
    // client settings;
    if (global.client.working_mode == CAPTURE_VIDEO) {
        dmd_log(LOG_INFO, "working_mode: capture video\n");
    } else if (global.client.working_mode == CAPTURE_PICTURE) {
        dmd_log(LOG_INFO, "working_mode: capture picture\n");
    } else if (global.client.working_mode == CAPTURE_ALL) {
        dmd_log(LOG_INFO, "working_mode: capture video and picture\n");
    } else {
        dmd_log(LOG_ERR, "Unsupported client working mode\n");
        return -1;
    }

    dmd_log(LOG_INFO, "video device:%s\n", global.client.video_device);
    dmd_log(LOG_INFO, "image width:%d\n", global.client.image_width);
    dmd_log(LOG_INFO, "image height:%d\n", global.client.image_height);
    dmd_log(LOG_INFO, "req count:%d\n", global.client.req_count);

    dmd_log(LOG_INFO, "diff pixels:%d\n", global.client.diff_pixels);
    dmd_log(LOG_INFO, "diff deviation:%d\n", global.client.diff_deviation);
    dmd_log(LOG_INFO, "video duration:%d\n", global.client.video_duration);

    if (global.client.picture_format == PICTURE_BMP) {
        dmd_log(LOG_INFO, "picture_format: bmp\n");
    } else if (global.client.picture_format == PICTURE_PNG) {
        dmd_log(LOG_INFO, "picture_format: png\n");
    } else if (global.client.picture_format == PICTURE_JPEG) {
        dmd_log(LOG_INFO, "picture_format: jpeg\n");
    } else {
        dmd_log(LOG_ERR, "Unsupported client picture format\n");
        return -1;
    }

    if (global.client.video_format == VIDEO_H264) {
        dmd_log(LOG_INFO, "video_format: h264\n");
    } else {
        dmd_log(LOG_ERR, "Unsupported client video format\n");
        return -1;
    }

    dmd_log(LOG_INFO, "client repo dir:%s\n", global.client.client_repo);

    dump_client_rtp();

    return 0;
}

static int dump_server()
{
    // server settings;
    dmd_log(LOG_INFO, "server repository dir:%s\n", global.server.server_repo);

    dmd_log(LOG_INFO, "client_scale:%d\n", global.server.client_scale);

    dmd_log(LOG_INFO, "server_ip:%s\n", global.server.server_ip);

    dmd_log(LOG_INFO, "server port base:%d\n", global.server.server_port_base);

    dmd_log(LOG_INFO, "server last_duration:%d\n", global.server.last_duration);

    return 0;
}

static int dump_common()
{
    // basic settings;
    if (global.cluster_mode == CLUSTER_CLIENT) {
        dmd_log(LOG_INFO, "cluster_mode: client\n");
    } else if (global.cluster_mode == CLUSTER_SERVER) {
        dmd_log(LOG_INFO, "cluster_mode: server\n");
    } else if (global.cluster_mode == CLUSTER_SINGLETON) {
        dmd_log(LOG_INFO, "cluster_mode: singleton\n");
    } else {
        dmd_log(LOG_ERR, "Unsupported cluster mode\n");
        return -1;
    }

    if (global.daemon_mode == DAEMON_ON) {
        dmd_log(LOG_INFO, "daemon_mode: on\n");
    } if (global.daemon_mode == DAEMON_OFF) {
        dmd_log(LOG_INFO, "daemon_mode: off\n");
    } else {
        dmd_log(LOG_ERR, "Unsupported daemon mode\n");
        return -1;
    }

    dmd_log(LOG_INFO, "pid file:%s\n", global.pid_file);
    dmd_log(LOG_INFO, "cfg file:%s\n", global.cfg_file);

    dmd_log(LOG_INFO, "x264_fps:%d\n", global.x264_fps);

    return 0;
}

int dump_global_config()
{
    // only dump, no error detect.
    dmd_log(LOG_INFO, "in function %s:\n", __func__);

    int ret = dump_common();
    assert(ret == 0);

    if (global.cluster_mode == CLUSTER_CLIENT
            || global.cluster_mode == CLUSTER_SINGLETON) {
        ret = dump_client();
        assert(ret == 0);
    } else if (global.cluster_mode == CLUSTER_SERVER) {
        ret = dump_server();
        assert(ret == 0);
    }

    return 0;
}


static void release_client()
{
    dmd_log(LOG_INFO, "at function %s, free malloced memory\n", __func__);

    // free reusable buffers;
    free(global.client.referenceYUYV422);
    global.client.referenceYUYV422 = NULL;

    free(global.client.rgbbuffer);
    global.client.rgbbuffer = NULL;

    free(global.client.pyuyv422buffer);
    global.client.pyuyv422buffer = NULL;

    free(global.client.vyuyv422buffer);
    global.client.vyuyv422buffer = NULL;

    free(global.client.yuv420pbuffer);
    global.client.yuv420pbuffer = NULL;

    free(global.client.bufferingYUYV422);
    global.client.bufferingYUYV422 = NULL;

    // wait worker thread;
    if (global.client.working_mode == CAPTURE_ALL) {
        pthread_join(global.client.thread_attr.picture_thread_id, NULL);
        pthread_join(global.client.thread_attr.video_thread_id, NULL);
    } else if (global.client.working_mode == CAPTURE_PICTURE) {
        pthread_join(global.client.thread_attr.picture_thread_id, NULL);
    } else if (global.client.working_mode == CAPTURE_VIDEO) {
        pthread_join(global.client.thread_attr.video_thread_id, NULL);
    } else {
        dmd_log(LOG_ERR, "in function %s, impossible reach here!\n", __func__);
        assert(0);
    }

    pthread_attr_destroy(&global.client.thread_attr.global_attr);
    pthread_rwlock_destroy(&global.client.thread_attr.bufferYUYV_rwlock);
    pthread_mutex_destroy(&global.client.thread_attr.picture_mutex);
    pthread_cond_destroy(&global.client.thread_attr.picture_cond);
    pthread_mutex_destroy(&global.client.thread_attr.video_mutex);
    pthread_cond_destroy(&global.client.thread_attr.video_cond);
}

static void release_server()
{
    return ;
}

// called at atexit() to free malloced memory in variable global;
void release_default_global()
{
    // clean threads;
    if (global.cluster_mode == CLUSTER_CLIENT ||
            global.cluster_mode == CLUSTER_SINGLETON) {
        release_client();
    } else {
        // TODO: master thread clean utils;
        release_server();
    }

    pthread_mutex_destroy(&total_thread_mutex);

    dmd_log(LOG_ERR, "in function %s, before dump stats!\n", __func__);
    // dump and release global statistics;
    pthread_mutex_lock(&global_stats->mutex);
    dump_statistics(global_stats);
    pthread_mutex_unlock(&global_stats->mutex);
    release_statistics(global_stats);
}
