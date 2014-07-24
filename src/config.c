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
 * File: config.c
 *
 * Brief: config file parsing;
 *
 * Date: 2014.05.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "config.h"


static void remove_tail_slash(char *path)
{
    dmd_log(LOG_DEBUG, "in function %s, before remove tail, path is %s\n",
            __func__, path);
    int len = strlen(path);
    // we only remove once, because path like "/home/abc/" may happen,
    // but path like "/home/abc//"may not happen always;
    if (*(path + len - 1) == '/') { // remove the tail slash;
        *(path + len - 1) = '\0';
    }
    dmd_log(LOG_DEBUG, "in function %s, after remove tail, path is %s\n",
            __func__, path);
}

static void check_path()
{
    remove_tail_slash(global.client.store_dir);
    remove_tail_slash(global.server.server_repo);
}

static int check_config_integrity()
{
    // client side check;
    if (global.cluster_mode == CLUSTER_CLIENT) {
        int lsn = global.client.clientrtp.local_sequence_number;
        int cs = global.server.client_scale;
        if (lsn <= 0 || lsn > cs)
            return -1;
    }

    check_path();

    return 0;

}

static void set_buffering()
{
    int width = global.client.image_width;
    int height = global.client.image_height;

    // rgb buffer;
    unsigned int rgblength = width * height * 3;
    global.client.rgbbuffer = (uint8_t *)malloc( rgblength * sizeof(uint8_t));
    assert(global.client.rgbbuffer != NULL);
    bzero(global.client.rgbbuffer, rgblength * sizeof(uint8_t));

    // referenceYUYV422 buffer;
    unsigned int referencelength = width * height * 2;
    global.client.referenceYUYV422 = (uint8_t *)malloc(
            referencelength * sizeof(uint8_t));
    assert(global.client.referenceYUYV422 != NULL);
    bzero(global.client.referenceYUYV422, referencelength * sizeof(uint8_t));

    // pyuyv422buffer;
    unsigned int pyuyv422length = width * height * 2;
    global.client.pyuyv422buffer = (uint8_t *)malloc(
            pyuyv422length * sizeof(uint8_t));
    assert(global.client.pyuyv422buffer != NULL);
    bzero(global.client.pyuyv422buffer, pyuyv422length * sizeof(uint8_t));

    // vyuyv422buffer;
    unsigned int vyuyv422length = width * height * 2;
    global.client.vyuyv422buffer = (uint8_t *)malloc(
            vyuyv422length * sizeof(uint8_t));
    assert(global.client.vyuyv422buffer != NULL);
    bzero(global.client.vyuyv422buffer, vyuyv422length * sizeof(uint8_t));

    // yuv420pbuffer;
    unsigned int yuv420plength = width * height * 1.5;
    global.client.yuv420pbuffer = (uint8_t *)malloc(
            yuv420plength * sizeof(uint8_t));
    assert(global.client.yuv420pbuffer != NULL);
    bzero(global.client.yuv420pbuffer, yuv420plength * sizeof(uint8_t));

    // bufferingYUYV422;
    unsigned int bufferyuyvlength = width * height * 2;
    global.client.bufferingYUYV422 = (uint8_t *)malloc(
            bufferyuyvlength * sizeof(uint8_t));
    assert(global.client.bufferingYUYV422 != NULL);
    bzero(global.client.bufferingYUYV422, bufferyuyvlength * sizeof(uint8_t));
}

int parse_config(const char *conf_file)
{
    char comment_char = '#';
    char separate_char = ' ';
    struct config *conf = new_config(comment_char, separate_char);
    int ret = parse_config_file(global.cfg_file, conf);
    assert(ret == 0);
    assert(conf->items != NULL);

    // heading node is not used;
    struct config_item *item = conf->items->next;
    while (item != NULL) {
        if (strcmp(item->key, "daemon_mode") == 0) {
            if (strcmp(item->value, "on") == 0) {
                global.daemon_mode = DAEMON_ON;
            } else if (strcmp(item->value, "off") == 0) {
                global.daemon_mode = DAEMON_OFF;
            } else {
                dmd_log(LOG_ERR, "invalid value of daemon_mode\n");
                return -1;
            }

        } else if (strcmp(item->key, "log_level") == 0) {
            if (strcmp(item->value, "LOG_INFO") == 0) {
                global.log_level = LOG_INFO;
            } else if (strcmp(item->value, "LOG_ERR") == 0) {
                global.log_level = LOG_ERR;
            } else if (strcmp(item->value, "LOG_DEBUG") == 0) {
                global.log_level = LOG_DEBUG;
            } else if (strcmp(item->value, "LOG_EMERG") == 0) {
                global.log_level = LOG_EMERG;
            } else if (strcmp(item->value, "LOG_ALERT") == 0) {
                global.log_level = LOG_ALERT;
            } else if (strcmp(item->value, "LOG_CRIT") == 0) {
                global.log_level = LOG_CRIT;
            } else if (strcmp(item->value, "LOG_WARNING") == 0) {
                global.log_level = LOG_WARNING;
            } else if (strcmp(item->value, "LOG_NOTICE") == 0) {
                global.log_level = LOG_NOTICE;
            } else {
                // impossible to reach here!
                dmd_log(LOG_ERR, "in file %s, function %s, line %s, "
                        "impossible to reach here!\n", __FILE__, __func__,
                        __LINE__);
                return -1;
            }

        } else if (strcmp(item->key, "cluster_mode") == 0) {
            if (strcmp(item->value, "singleton") == 0) {
                global.cluster_mode = CLUSTER_SINGLETON;
            } else if (strcmp(item->value, "client") == 0) {
                global.cluster_mode = CLUSTER_CLIENT;
            } else if (strcmp(item->value, "server") == 0) {
                global.cluster_mode = CLUSTER_SERVER;
            } else {
                dmd_log(LOG_ERR, "invalid value of cluster_mode\n");
                return -1;
            }

        } else if (strcmp(item->key, "local_ip") == 0) {
            assert(strlen(item->value) < PATH_MAX);
            strncpy(global.client.clientrtp.local_ip,
                    item->value, strlen(item->value));
            global.client.clientrtp.local_ip[strlen(item->value)] = '\0';
        } else if (strcmp(item->key, "local_port") == 0) {
            int port = atoi(item->value);
            assert(port >= 0 && port <= 65535);
            global.client.clientrtp.local_port = port;
        } else if (strcmp(item->key, "local_sequence_number") == 0) {
            int local_sequence_number = atoi(item->value);
            assert(local_sequence_number > 0);
            global.client.clientrtp.local_sequence_number =
                local_sequence_number;

        } else if (strcmp(item->key, "server_ip") == 0) {
            assert(strlen(item->value) < PATH_MAX);
            strncpy(global.client.clientrtp.server_ip,
                    item->value, strlen(item->value));
            global.client.clientrtp.server_ip[strlen(item->value)] = '\0';

            // also available for server end;
            strncpy(global.server.server_ip, item->value, strlen(item->value));
            global.server.server_ip[strlen(item->value)] = '\0';

        } else if (strcmp(item->key, "server_rtp_port") == 0) {
            int port = atoi(item->value);
            assert(port >= 0 && port <= 65535);
            global.client.clientrtp.server_rtp_port = port;
        } else if (strcmp(item->key, "server_port_base") == 0) {
            int portbase = atoi(item->value);
            assert(portbase >= 0 && portbase <= 65535);
            global.client.clientrtp.server_port_base = portbase;

            // also available for server end;
            global.server.server_port_base = portbase;

        } else if (strcmp(item->key, "server_rtcp_port") == 0) {
            int port = atoi(item->value);
            assert(port >= 0 && port <= 65535);
            global.client.clientrtp.server_rtcp_port = port;

        } else if (strcmp(item->key, "client_scale") == 0) {
            int client_scale = atoi(item->value);
            assert(client_scale > 0);
            global.server.client_scale = client_scale;
        } else if (strcmp(item->key, "last_duration") == 0) {
            int last_duration = atoi(item->value);
            assert(last_duration > 0);
            global.server.last_duration = last_duration;

        } else if (strcmp(item->key, "pid_file") == 0) {
            assert(strlen(item->value) < PATH_MAX);
            strncpy(global.pid_file, item->value, strlen(item->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.pid_file[strlen(item->value)] = '\0';

        } else if (strcmp(item->key, "x264_fps") == 0) {
            int x264_fps = atoi(item->value);
            assert(x264_fps > 0);
            global.x264_fps = x264_fps;

        } else if (strcmp(item->key, "working_mode") == 0) {
            if (strcmp(item->value, "picture") == 0) {
                global.client.working_mode = CAPTURE_PICTURE;
            } else if (strcmp(item->value, "video") == 0) {
                global.client.working_mode = CAPTURE_VIDEO;
            } else if (strcmp(item->value, "all") == 0) {
                global.client.working_mode = CAPTURE_ALL;
            } else {
                dmd_log(LOG_ERR, "invalid value of working_mode\n");
                return -1;
            }

        } else if (strcmp(item->key, "video_device") == 0) {
            assert(strlen(item->value) < PATH_MAX);
            strncpy(global.client.video_device,
                    item->value, strlen(item->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.client.video_device[strlen(item->value)] = '\0';

        } else if (strcmp(item->key, "image_width") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_width = atoi(item->value);

        } else if (strcmp(item->key, "image_height") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_height = atoi(item->value);

        } else if (strcmp(item->key, "req_count") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.req_count = atoi(item->value);

        } else if (strcmp(item->key, "diff_pixels") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.diff_pixels = atoi(item->value);

        } else if (strcmp(item->key, "diff_deviation") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.diff_deviation = atoi(item->value);

        } else if (strcmp(item->key, "video_duration") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.video_duration = atoi(item->value);

        } else if (strcmp(item->key, "picture_format") == 0) {
            if (strcmp(item->value, "bmp") == 0) {
                global.client.picture_format = PICTURE_BMP;
            } else if (strcmp(item->value, "png") == 0) {
                global.client.picture_format = PICTURE_PNG;
            } else if (strcmp(item->value, "jpeg") == 0) {
                global.client.picture_format = PICTURE_JPEG;
            } else {
                dmd_log(LOG_ERR, "invalid value of picture_format\n");
                return -1;
            }

        } else if (strcmp(item->key, "video_device") == 0) {
            assert(strlen(item->value) < PATH_MAX);
            strncpy(global.client.video_device,
                    item->value, strlen(item->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.client.video_device[strlen(item->value)] = '\0';

        } else if (strcmp(item->key, "image_width") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_width = atoi(item->value);

        } else if (strcmp(item->key, "image_height") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_height = atoi(item->value);

        } else if (strcmp(item->key, "video_format") == 0) {
            if (strcmp(item->value, "h264") == 0) {
                global.client.video_format = VIDEO_H264;
            } else {
                dmd_log(LOG_ERR, "invalid value of video_format\n");
                return -1;
            }

        } else if (strcmp(item->key, "store_dir") == 0) {
            assert(strlen(item->value) < PATH_MAX);
            strncpy(global.client.store_dir, item->value, strlen(item->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.client.store_dir[strlen(item->value)] = '\0';

        } else if (strcmp(item->key, "server_repo") == 0) {
            assert(strlen(item->value) < PATH_MAX);
            strncpy(global.server.server_repo,
                    item->value, strlen(item->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.server.server_repo[strlen(item->value)] = '\0';

        } else {
            dmd_log(LOG_ERR, "unsupported parameter:%s \n", item->key);
            return -1;
        }

        item = item->next;
    } // while


    // clean the config parser;
    release_config(conf);

    // after config parse, check config integrity;
    ret = check_config_integrity();
    if (ret != 0) {
        dmd_log(LOG_ERR, "in function %s, "
                "config file has some integrity problem\n", __func__);
        return -1;
    }

    // after image_width and image_height are finally determined,
    // set reusable image buffers;
    set_buffering();

    return 0;
}
