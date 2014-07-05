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
 * File: flv_muxer.h
 *
 * Brief: encapsulate h264 raw video frames to flv muxer;
 *
 * Date: 2014.07.02
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef FLV_MUXER_H
#define FLV_MUXER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "log.h"
#include "global_context.h"


/*
 * an flv file format:
 * flv_header                 // 9 bytes;
 * previous_tag_size 0;       // 4 bytes;
 * tag 1;                     // flv tag 1;
 * previous_tag_size 1;       // size of flv tag 1;
 * ......
 * previous_tag_size n - 1;   // size of flv tag n - 1;
 * tag n;
 * previous_tag_size n;
 *
 *
 *
 * ATTENTION: in AAC/AVC audio/video, tag 1 always to be script tag;
 *            namely, a script tag containing first AMF and second AMF;
 */

extern uint8_t flv_header[13];
extern uint8_t first_AMF[13];

struct tag_header {
    uint8_t tag_type;             /* 0x08 audio, 0x09 video, ox12 script */
    uint8_t tag_data_len[3];
    uint8_t timestamp[3];
    uint8_t extending_timestamp;
    uint8_t stream_id[3];
};

#define FLV_TAG_HEADER_SIZE 11
#define FLV_PRE_TAG_SIZE 4

// encapulate flv header;
extern int encapulate_flvheader(const char *filename);

extern int encapulate_first_tag(const char *filename);

// encapulate first tag body;
extern int encapulate_spspps(uint8_t *sps, int sps_len,
        uint8_t *pps, int pps_len, const char *filename);

// encapulate IDR/SLICE nalu;
extern int encapulate_nalu(uint8_t *nalu, int nalu_len, const char *filename);

#endif

