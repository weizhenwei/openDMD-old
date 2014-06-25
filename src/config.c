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

static int check_config_integrity()
{
    // client side check;
    if (global.cluster_mode == CLUSTER_CLIENT) {
        int lsn = global.client.clientrtp.local_sequence_number;
        int cs = global.server.client_scale;
        if (lsn <= 0 || lsn > cs)
            return -1;
    }

    return 0;

}

static void set_buffering()
{
    int width = global.client.image_width;
    int height = global.client.image_height;

    // rgb buffer;
    unsigned int rgblength = width * height * 3;
    global.client.rgbbuffer = (unsigned char *)malloc(
        rgblength * sizeof(unsigned char));
    assert(global.client.rgbbuffer != NULL);
    bzero(global.client.rgbbuffer, rgblength * sizeof(unsigned char));

    // referenceYUYV422 buffer;
    unsigned int referencelength = width * height * 2;
    global.client.referenceYUYV422 = (unsigned char *)malloc(
            referencelength * sizeof(unsigned char));
    assert(global.client.referenceYUYV422 != NULL);
    bzero(global.client.referenceYUYV422,
            referencelength * sizeof(unsigned char));

    // pyuyv422buffer;
    unsigned int pyuyv422length = width * height * 2;
    global.client.pyuyv422buffer = (unsigned char *)malloc(
            pyuyv422length * sizeof(unsigned char));
    assert(global.client.pyuyv422buffer != NULL);
    bzero(global.client.pyuyv422buffer,
            pyuyv422length * sizeof(unsigned char));

    // vyuyv422buffer;
    unsigned int vyuyv422length = width * height * 2;
    global.client.vyuyv422buffer = (unsigned char *)malloc(
            vyuyv422length * sizeof(unsigned char));
    assert(global.client.vyuyv422buffer != NULL);
    bzero(global.client.vyuyv422buffer,
            vyuyv422length * sizeof(unsigned char));

    // yuv420pbuffer;
    unsigned int yuv420plength = width * height * 1.5;
    global.client.yuv420pbuffer = (unsigned char *)malloc(
            yuv420plength * sizeof(unsigned char));
    assert(global.client.yuv420pbuffer != NULL);
    bzero(global.client.yuv420pbuffer,
            yuv420plength * sizeof(unsigned char));

    // bufferingYUYV422;
    unsigned int bufferyuyvlength = width * height * 2;
    global.client.bufferingYUYV422 = (unsigned char *)malloc(
            bufferyuyvlength * sizeof(unsigned char));
    assert(global.client.bufferingYUYV422 != NULL);
    bzero(global.client.bufferingYUYV422,
            bufferyuyvlength * sizeof(unsigned char));

}

int parse_config(const char *conf_file)
{
    struct ccl_t config;
    const struct ccl_pair_t *iter;

    // set config parsing control character;
    config.comment_char = '#';
    config.sep_char = ' ';
    config.str_char = '"';

    // parse the config file;
    ccl_parse(&config, conf_file);

    // key/value pairs is sorted in ascending order according to key.
    while ((iter = ccl_iterate(&config)) != 0) {
        if (strcmp(iter->key, "daemon_mode") == 0) {
            if (strcmp(iter->value, "on") == 0) {
                global.daemon_mode = DAEMON_ON;
            } else if (strcmp(iter->value, "off") == 0) {
                global.daemon_mode = DAEMON_OFF;
            } else {
                dmd_log(LOG_ERR, "invalid value of daemon_mode\n");
                return -1;
            }

        } else if (strcmp(iter->key, "log_level") == 0) {
            if (strcmp(iter->value, "LOG_INFO") == 0) {
                global.log_level = LOG_INFO;
            } else if (strcmp(iter->value, "LOG_ERR") == 0) {
                global.log_level = LOG_ERR;
            } else if (strcmp(iter->value, "LOG_DEBUG") == 0) {
                global.log_level = LOG_DEBUG;
            } else if (strcmp(iter->value, "LOG_EMERG") == 0) {
                global.log_level = LOG_EMERG;
            } else if (strcmp(iter->value, "LOG_ALERT") == 0) {
                global.log_level = LOG_ALERT;
            } else if (strcmp(iter->value, "LOG_CRIT") == 0) {
                global.log_level = LOG_CRIT;
            } else if (strcmp(iter->value, "LOG_WARNING") == 0) {
                global.log_level = LOG_WARNING;
            } else if (strcmp(iter->value, "LOG_NOTICE") == 0) {
                global.log_level = LOG_NOTICE;
            } else {
                // impossible to reach here!
                dmd_log(LOG_ERR, "in file %s, function %s, line %s, "
                        "impossible to reach here!\n", __FILE__, __func__,
                        __LINE__);
                return -1;
            }

        } else if (strcmp(iter->key, "cluster_mode") == 0) {
            if (strcmp(iter->value, "singleton") == 0) {
                global.cluster_mode = CLUSTER_SINGLETON;
            } else if (strcmp(iter->value, "client") == 0) {
                global.cluster_mode = CLUSTER_CLIENT;
            } else if (strcmp(iter->value, "server") == 0) {
                global.cluster_mode = CLUSTER_SERVER;
            } else {
                dmd_log(LOG_ERR, "invalid value of cluster_mode\n");
                return -1;
            }

        } else if (strcmp(iter->key, "local_ip") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.client.clientrtp.local_ip,
                    iter->value, strlen(iter->value));
            global.client.clientrtp.local_ip[strlen(iter->value)] = '\0';
        } else if (strcmp(iter->key, "local_port") == 0) {
            int port = atoi(iter->value);
            assert(port >= 0 && port <= 65535);
            global.client.clientrtp.local_port = port;
        } else if (strcmp(iter->key, "local_sequence_number") == 0) {
            int local_sequence_number = atoi(iter->value);
            assert(local_sequence_number > 0);
            global.client.clientrtp.local_sequence_number =
                local_sequence_number;

        } else if (strcmp(iter->key, "server_ip") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.client.clientrtp.server_ip,
                    iter->value, strlen(iter->value));
            global.client.clientrtp.server_ip[strlen(iter->value)] = '\0';

            // also available for server end;
            strncpy(global.server.server_ip, iter->value, strlen(iter->value));
            global.server.server_ip[strlen(iter->value)] = '\0';

        } else if (strcmp(iter->key, "server_rtp_port") == 0) {
            int port = atoi(iter->value);
            assert(port >= 0 && port <= 65535);
            global.client.clientrtp.server_rtp_port = port;
        } else if (strcmp(iter->key, "server_port_base") == 0) {
            int portbase = atoi(iter->value);
            assert(portbase >= 0 && portbase <= 65535);
            global.client.clientrtp.server_port_base = portbase;

            // also available for server end;
            global.server.server_port_base = portbase;

        } else if (strcmp(iter->key, "server_rtcp_port") == 0) {
            int port = atoi(iter->value);
            assert(port >= 0 && port <= 65535);
            global.client.clientrtp.server_rtcp_port = port;

        } else if (strcmp(iter->key, "client_scale") == 0) {
            int client_scale = atoi(iter->value);
            assert(client_scale > 0);
            global.server.client_scale = client_scale;
        } else if (strcmp(iter->key, "last_duration") == 0) {
            int last_duration = atoi(iter->value);
            assert(last_duration > 0);
            global.server.last_duration = last_duration;

        } else if (strcmp(iter->key, "pid_file") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.pid_file, iter->value, strlen(iter->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.pid_file[strlen(iter->value)] = '\0';

        } else if (strcmp(iter->key, "working_mode") == 0) {
            if (strcmp(iter->value, "picture") == 0) {
                global.client.working_mode = CAPTURE_PICTURE;
            } else if (strcmp(iter->value, "video") == 0) {
                global.client.working_mode = CAPTURE_VIDEO;
            } else if (strcmp(iter->value, "all") == 0) {
                global.client.working_mode = CAPTURE_ALL;
            } else {
                dmd_log(LOG_ERR, "invalid value of working_mode\n");
                return -1;
            }

        } else if (strcmp(iter->key, "video_device") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.client.video_device,
                    iter->value, strlen(iter->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.client.video_device[strlen(iter->value)] = '\0';

        } else if (strcmp(iter->key, "image_width") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_width = atoi(iter->value);

        } else if (strcmp(iter->key, "image_height") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_height = atoi(iter->value);

        } else if (strcmp(iter->key, "req_count") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.req_count = atoi(iter->value);

        } else if (strcmp(iter->key, "diff_pixels") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.diff_pixels = atoi(iter->value);

        } else if (strcmp(iter->key, "diff_deviation") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.diff_deviation = atoi(iter->value);

        } else if (strcmp(iter->key, "video_duration") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.video_duration = atoi(iter->value);

        } else if (strcmp(iter->key, "picture_format") == 0) {
            if (strcmp(iter->value, "bmp") == 0) {
                global.client.picture_format = PICTURE_BMP;
            } else if (strcmp(iter->value, "png") == 0) {
                global.client.picture_format = PICTURE_PNG;
            } else if (strcmp(iter->value, "jpeg") == 0) {
                global.client.picture_format = PICTURE_JPEG;
            } else {
                dmd_log(LOG_ERR, "invalid value of picture_format\n");
                return -1;
            }

        } else if (strcmp(iter->key, "video_device") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.client.video_device,
                    iter->value, strlen(iter->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.client.video_device[strlen(iter->value)] = '\0';

        } else if (strcmp(iter->key, "image_width") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_width = atoi(iter->value);

        } else if (strcmp(iter->key, "image_height") == 0) {
            // Waring: there is no error detection in atoi();
            global.client.image_height = atoi(iter->value);

        } else if (strcmp(iter->key, "video_format") == 0) {
            if (strcmp(iter->value, "h264") == 0) {
                global.client.video_format = VIDEO_H264;
            } else {
                dmd_log(LOG_ERR, "invalid value of video_format\n");
                return -1;
            }

        } else if (strcmp(iter->key, "store_dir") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.client.store_dir, iter->value, strlen(iter->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.client.store_dir[strlen(iter->value)] = '\0';

        } else if (strcmp(iter->key, "server_repo") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.server.server_repo,
                    iter->value, strlen(iter->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.server.server_repo[strlen(iter->value)] = '\0';

        } else {
            dmd_log(LOG_ERR, "unsupported parameter:%s \n", iter->key);
            return -1;
        }
    } // while

    ccl_release(&config);

    // after config parse, check config integrity;
    int ret = check_config_integrity();
    assert(ret == 0);

    // after image_width and image_height are finally determined,
    // set reusable image buffers;
    set_buffering();


    return 0;
}
