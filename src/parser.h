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
 * File: parser.h
 *
 * Brief: parse the config file.
 *
 * Date: 2014.07.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <strings.h>

#include "log.h"

// config item type, for optimization parsing process;
enum config_item_type {
    CONFIG_ITEM_NULL = 0,

    // global basic settngs
    CLUSTER_MODE_T = 1,
    LOG_LEVEL_T = 2,
    DAEMON_MODE_T = 3,
    PID_FILE_T = 4,
    X264_FPS_T = 5,

    // client settings
    WORKING_MODE_T = 6,
    VIDEO_DEVICE_T = 7,
    IMAGE_WIDTH_T = 8,
    IMAGE_HEIGHT_T = 9,
    REQ_COUNT_T = 10,
    DIFF_PIXELS_T = 11,
    DIFF_DEVIATION_T = 12,
    VIDEO_DURATION_T = 13,
    PICTURE_FORMAT_T = 14,
    VIDEO_FORMAT_T = 15,
    STORE_DIR_T = 16,
    

    // rtp session settings
    LOCAL_IP_T = 17,
    LOCAL_PORT_T = 18,
    LOCAL_SEQUENCE_NUMBER_T = 19,

    SERVER_IP_T = 20,
    SERVER_PORT_BASE_T = 21,
    SERVER_RTP_PORT_T = 22,
    SERVER_RTCP_PORT_T = 23,
    
    // server settings;
    SERVER_REPO_T = 24,
    CLIENT_SCALE_T = 25,
    LAST_DURATION_T = 26,
};

struct config_item {
    struct config_item *next;
    char *key;
    char *value;
};

struct config {
    char comment_char;
    char separator_char;
    unsigned int total_item;
    struct config_item *items;
    struct config_item *tail;
};

extern struct config *new_config(const char comment_char,
        const char separator_char);

extern int parse_config_file(const char *config_file, struct config *conf);

extern void dump_config(const struct config *conf);

extern void release_config(struct config *conf);

#endif
