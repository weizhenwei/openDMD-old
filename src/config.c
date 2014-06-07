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

static void set_buffering()
{
    int width = global.image_width;
    int height = global.image_height;

    // rgb buffer;
    unsigned int rgblength = width * height * 3;
    global.rgbbuffer = (unsigned char *)malloc(
        rgblength * sizeof(unsigned char));
    assert(global.rgbbuffer != NULL);
    bzero(global.rgbbuffer, rgblength * sizeof(unsigned char));

    // referenceYUYV422 buffer;
    unsigned int referencelength = width * height * 2;
    global.referenceYUYV422 = (unsigned char *)malloc(
            referencelength * sizeof(unsigned char));
    assert(global.referenceYUYV422 != NULL);
    bzero(global.referenceYUYV422, referencelength * sizeof(unsigned char));

    // pyuyv422buffer;
    unsigned int pyuyv422length = width * height * 2;
    global.pyuyv422buffer = (unsigned char *)malloc(
            pyuyv422length * sizeof(unsigned char));
    assert(global.pyuyv422buffer != NULL);
    bzero(global.pyuyv422buffer, pyuyv422length * sizeof(unsigned char));

    // vyuyv422buffer;
    unsigned int vyuyv422length = width * height * 2;
    global.vyuyv422buffer = (unsigned char *)malloc(
            vyuyv422length * sizeof(unsigned char));
    assert(global.vyuyv422buffer != NULL);
    bzero(global.vyuyv422buffer, vyuyv422length * sizeof(unsigned char));

    // yuv420pbuffer;
    unsigned int yuv420plength = width * height * 1.5;
    global.yuv420pbuffer = (unsigned char *)malloc(
            yuv420plength * sizeof(unsigned char));
    assert(global.yuv420pbuffer != NULL);
    bzero(global.yuv420pbuffer, yuv420plength * sizeof(unsigned char));

    // bufferingYUYV422;
    unsigned int bufferyuyvlength = width * height * 2;
    global.bufferingYUYV422 = (unsigned char *)malloc(
            bufferyuyvlength * sizeof(unsigned char));
    assert(global.bufferingYUYV422 != NULL);
    bzero(global.bufferingYUYV422, bufferyuyvlength * sizeof(unsigned char));

}

void parse_config(const char *conf_file)
{
    struct ccl_t config;
    const struct ccl_pair_t *iter;

    // set config parsing control character;
    config.comment_char = '#';
    config.sep_char = ' ';
    config.str_char = '"';

    dmd_log(LOG_INFO, "in %s: config file is %s\n", __func__, conf_file);

    // parse the config file;
    ccl_parse(&config, conf_file);

    // key/value pairs is sorted in ascending order according to key.
    while ((iter = ccl_iterate(&config)) != 0) {
        dmd_log(LOG_INFO, "key: %s, value: %s\n", iter->key, iter->value);

        if (strcmp(iter->key, "daemon_mode") == 0) {
            if (strcmp(iter->value, "on") == 0) {
                global.daemon_mode = DAEMON_ON;
            } else if (strcmp(iter->value, "off") == 0) {
                global.daemon_mode = DAEMON_OFF;
            } else {
                dmd_log(LOG_ERR, "invalid value of daemon_mode\n");
            }
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
                global.working_mode = CAPTURE_PICTURE;
            } else if (strcmp(iter->value, "video") == 0) {
                global.working_mode = CAPTURE_VIDEO;
            } else if (strcmp(iter->value, "all") == 0) {
                global.working_mode = CAPTURE_ALL;
            } else {
                dmd_log(LOG_ERR, "invalid value of working_mode\n");
            }
        } else if (strcmp(iter->key, "video_device") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.video_device, iter->value, strlen(iter->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.video_device[strlen(iter->value)] = '\0';
        } else if (strcmp(iter->key, "image_width") == 0) {
            // Waring: there is no error detection in atoi();
            global.image_width = atoi(iter->value);
        } else if (strcmp(iter->key, "image_height") == 0) {
            // Waring: there is no error detection in atoi();
            global.image_height = atoi(iter->value);
        } else if (strcmp(iter->key, "req_count") == 0) {
            // Waring: there is no error detection in atoi();
            global.req_count = atoi(iter->value);
        } else if (strcmp(iter->key, "diff_pixels") == 0) {
            // Waring: there is no error detection in atoi();
            global.diff_pixels = atoi(iter->value);
        } else if (strcmp(iter->key, "diff_deviation") == 0) {
            // Waring: there is no error detection in atoi();
            global.diff_deviation = atoi(iter->value);
        } else if (strcmp(iter->key, "video_duration") == 0) {
            // Waring: there is no error detection in atoi();
            global.video_duration = atoi(iter->value);
        } else if (strcmp(iter->key, "picture_format") == 0) {
            if (strcmp(iter->value, "bmp") == 0) {
                global.picture_format = PICTURE_BMP;
            } else if (strcmp(iter->value, "png") == 0) {
                global.picture_format = PICTURE_PNG;
            } else if (strcmp(iter->value, "jpeg") == 0) {
                global.picture_format = PICTURE_JPEG;
            } else {
                dmd_log(LOG_ERR, "invalid value of picture_format\n");
            }
        } else if (strcmp(iter->key, "video_format") == 0) {
            if (strcmp(iter->value, "h264") == 0) {
                global.video_format = VIDEO_H264;
            } else {
                dmd_log(LOG_ERR, "invalid value of video_format\n");
            }
        } else if (strcmp(iter->key, "store_dir") == 0) {
            assert(strlen(iter->value) < PATH_MAX);
            strncpy(global.store_dir, iter->value, strlen(iter->value));
            /* Warning:If there is no null byte among the first n bytes of
             * src, the string placed in dest will not be null-terminated,
             * remember add null-terminated manually.
             */
            global.store_dir[strlen(iter->value)] = '\0';
        } else {
            dmd_log(LOG_ERR, "unsupported parameter:%s \n", iter->key);
        }
    } // while

    // after image_width and image_height are finally determined,
    // set reusable image buffers;
    set_buffering();

    ccl_release(&config);
}
