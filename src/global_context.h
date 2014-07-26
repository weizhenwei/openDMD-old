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
 * File: global_context.h
 *
 * Brief: struct global_context is used to control opendmd running.
 *
 * Date: 2014.05.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <linux/limits.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>

#include <ortp/ortp.h>

#include "log.h"

/////////////////////////////////////////////////////////////
// Common default settings                                 //
/////////////////////////////////////////////////////////////
#define DEFAULT_RELEASE_PID_FILE "/var/run/opendmd/opendmd.pid"
// #define DEFAULT_DEBUG_PID_FILE "/home/wzw/opendmd/opendmd.pid"
#define DEFAULT_DEBUG_PID_FILE "opendmd.pid"

#define DEFAULT_RELEASE_CFG_FILE "/var/run/opendmd/opendmd.cfg"
// #define DEFAULT_DEBUG_CFG_FILE "/home/wzw/opendmd/opendmd.cfg"
#define DEFAULT_DEBUG_CFG_FILE "config/opendmd.cfg"

#define LOCAL_IP "0.0.0.0"
#define LOCAL_PORT 8000
#define LOCAL_SEQUENCE_NUMBER 1

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT_BASE 5000
#define CLIENT_SCALE 5


/////////////////////////////////////////////////////////////
// Client default settings                                 //
/////////////////////////////////////////////////////////////

#define DEFAULT_VIDEO_DEVICE "/dev/video0"

#define DEFAULT_VIDEO_WIDTH 640
#define DEFAULT_VIDEO_HEIGHT 480
#define DEFAULT_REQCOUNT 5


#define DEFAULT_DIFF_PIXELS 3000
#define DEFAULT_DIFF_DEVIATION 20

#define DEFAULT_RELEASE_STORE_DIR "/tmp/opendmd"
#define DEFAULT_DEBUG_STORE_DIR "/home/wzw/opendmd/client_repo"

#define DEFAULT_X264_FPS 25


/////////////////////////////////////////////////////////////
// Server default settings                                 //
/////////////////////////////////////////////////////////////

#define SERVER_REPO "/home/wzw/opendmd/server_repo"

// declare the global variable global to control opendmd running.
extern struct global_context global;

// opendmd run in daemon mode;
enum daemon_mode_type {
    DAEMON_ON = 1,
    DAEMON_OFF = 2,
};

// the opendmd working mode: capture picture, capture video or capture all;
enum working_type {
    CAPTURE_PICTURE = 1,
    CAPTURE_VIDEO = 2,
    CAPTURE_ALL = 3,
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

struct thread_attribute {
    pthread_attr_t global_attr;          // global shared thread attribute;

    pthread_rwlock_t bufferYUYV_rwlock;  // rwlock for r/w bufferingYUYV422;

    pthread_mutex_t picture_mutex;       // mutex for picture thread waiting
                                         // condtion from main thread;
    pthread_cond_t picture_cond;         // cond for main thread notifying
                                         // picture thread;
    pthread_mutex_t video_mutex;         // mutex for video thread waiting
                                         // condtion from main thread;
    pthread_cond_t video_cond;           // cond for main thread notifying
                                         // video thread;
    pthread_t picture_thread_id;         // picture thread id;
    pthread_t video_thread_id;           // video thread id;
};

// notify target: main thread chose to notify picture thread or video thread,
// or notify both;
enum main_notify_target {
    NOTIFY_NONE = 0, /* initial value */
    NOTIFY_PICTURE = 1,
    NOTIFY_VIDEO = 2,
    NOTIFY_ALL = 3,
    NOTIFY_EXIT = 4,
};

// cluster mode: slave, master, or singleton
// client: as a working node, responsible for capturing and sending;
// server: as a master node, responsible for receiving and saving;
// singleton: only one node in whole cluster , capturing and saving;
enum cluster_mode_t {
    CLUSTER_CLIENT = 1,
    CLUSTER_SERVER = 2,
    CLUSTER_SINGLETON = 3,
};

// for client sending rtp packet to server;
struct client_rtp {
    RtpSession *rtpsession;
    uint32_t user_ts;

    // local ip and port for symmetric communication;
    char local_ip[PATH_MAX];
    int local_port;
    int local_sequence_number;    // client sequence number in cluster;
                                  // valid range:[1, client_scale],
                                  // client_scale defined in server_context;

    // server port and port for multiple session receiving;
    char server_ip[PATH_MAX];
    /*
     * server port_base is the base port for rtp_port and rtcp_port allocation;
     * server_rtp_port = server_port_base + 2 * local_sequence_number
     * server_rtcp_port = server_port_base + 2 * local_sequence_number + 1;
     * server is using session_set_select() to do Multiplex I/O;
     */
    int server_port_base;         // the start port of the server port;
    int server_rtp_port;
    int server_rtcp_port;
};

struct client_context {
    // basic client settings;
    enum working_type  working_mode;     // working mode: picture, video or all;

    // video device settings;
    char video_device[PATH_MAX];         // video device path;
    unsigned int image_width;            // image width in pixels; 
    unsigned int image_height;           // image height in pixels; 
    unsigned int req_count;              // mmap req.count;

    // motion detection threshold settings;
    unsigned int diff_pixels;         // the pixels threshold motion occured;
    unsigned int diff_deviation;      // the deviation pixel allowed;
    time_t lasttime;                  // last time the motion detected;
    unsigned int counter_in_second;   // the movments detected in a second,
                                      // used for picture capturing;
    unsigned int video_duration;      // time elapsed when last motion detected
                                      // before video capturing stoped;
                                      // (in seconds);

    // reusable image buffer;
    uint8_t *referenceYUYV422;        // reference image when detect motion;
                                      // length = image_width*image_height*2
    uint8_t *rgbbuffer;               // rgb buffer used in picture capturing;
                                      // length = image_width*image_height*3;
    uint8_t *pyuyv422buffer;          // yuyv422buffer used in picture capture;
                                      // length = image_width*image_height*2;
    uint8_t *vyuyv422buffer;          // yuyv422buffer used in video capture;
                                      // length = image_width*image_height*2;
    uint8_t *yuv420pbuffer;           // yuyv420pbuffer used in video capture;
                                      // length = image_width*image_height*1.5;
    uint8_t *bufferingYUYV422;        // buffered place when captured new image
                                      // length = image_width*image_height*2;

    struct thread_attribute thread_attr;    // thread attribute;
    enum main_notify_target picture_target; // picture thread notify target;
    enum main_notify_target video_target;   // picture thread notify target;

    // captured pictures/video storage settings;
    enum picture_format_type picture_format; // captured picture format.
    enum video_format_type video_format;     // captured video format;
    char client_repo[PATH_MAX];              // captured pictures/video
                                             // storage directory.
    // video sending ortp associated
    struct client_rtp clientrtp;                   // client ortp session;
};

// describe a client in server side;
struct server_client_item {
    RtpSession *rtpsession;   // rtpsession used for send/recv;
    time_t lasttime;          // last time when received from this client;
    FILE *fp;                 // fp for writing to disk file;
    char filename[PATH_MAX];  // disk filename for writing to;
    
};

struct server_context {
    char server_repo[PATH_MAX];              // captured pictures/video
                                             // storage directory.
    uint32_t user_ts;                        // user_ts for receiving client;
    int client_scale;                        // total number of client node;
    char server_ip[PATH_MAX];                // ortp server ip;
    int server_port_base;                    // base port for rtp_port
                                             // and rtcp_port mallocation;

    time_t last_duration;                    // if time elapsed this period,
                                             // but no receiving from client,
                                             // restart fp for next filename;
    struct server_client_item *client_items; // total client items,
                                             // totalnumber = client_scale;
};

struct global_context {
    // Basic global information
    pid_t pid;                 // main thread's pid;

    // these are defined in syslog.h
    // #define LOG_EMERG   0   /* system is unusable */
    // #define LOG_ALERT   1   /* action must be taken immediately */
    // #define LOG_CRIT    2   /* critical conditions */
    // #define LOG_ERR     3   /* error conditions */
    // #define LOG_WARNING 4   /* warning conditions */
    // #define LOG_NOTICE  5   /* normal but significant condition */
    // #define LOG_INFO    6   /* informational */
    // #define LOG_DEBUG   7   /* debug-level messages */
    unsigned int log_level;    // dmd_log level, equivelent to syslog level;

    enum cluster_mode_t cluster_mode;    // cluster mode;

    enum daemon_mode_type daemon_mode;   // run in daemon mode;
    char pid_file[PATH_MAX];             // main process's pid file;
    char cfg_file[PATH_MAX];             // config file;

    // x264 fps setting
    int x264_fps;

    // client/server settings;
    struct client_context client;
    struct server_context server;
};

extern void init_default_global();

extern int dump_global_config();


// called at atexit() to free malloced memory in variable global;
extern void release_default_global();

#endif
