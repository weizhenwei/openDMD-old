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

#include <stdint.h>


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

uint8_t flv_header[9] = {
    0x46, 0x4c, 0x56, /* file type, namely string "FLV" */
    0x01,             /* version number, always to be 0x01 */
    0x01,             /* stream info: 0x01 is video, 0x04 audio, 0x05 both */
    0x00, 0x00, 0x00, 0x09 /* flv header length, always to be 0x09 */
};

struct tag_header {
    uint8_t previous_tag_size[4];
    uint8_t tag_type;             /* 0x08 audio, 0x09 video, ox12 script */
    uint8_t tag_data_len[3];
    uint8_t timestamp[3];
    uint8_t extending_timestamp;
    uint8_t stream_id[3];
};

uint8_t first_AMF[13] = {
    0x02, /* AMF package type: AMF_DATA_TYPE_STRING */
    0x00, 0x0a, /* package data len, always to be 10 */
    0x6f, 0x6e, /* string "on" */
    0x4d, 0x65, 0x74, 0x61, /* string "Meta" */
    0x44, 0x61, 0x74, 0x61  /* string "Data" */
};

#endif

