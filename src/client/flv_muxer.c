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
 * File: flv_muxer.c
 *
 * Brief: encapsulate h264 raw video frames to flv muxer;
 *
 * Date: 2014.07.02
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "src/client/flv_muxer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <x264.h>

#include "src/global_context.h"
#include "src/log.h"


// 9 bytes header + 4 bytes previous tag size;
uint8_t flv_header[13] = {
    0x46, 0x4c, 0x56,  /* file type, namely string "FLV" */
    0x01,              /* version number, always to be 0x01 */
    0x01,              /* stream info: 0x01 is video, 0x04 audio, 0x05 both */
    0x00, 0x00, 0x00, 0x09,  /* flv header length, always to be 0x09 */
    0x00, 0x00, 0x00, 0x00,  /* previous tag size, always to be 0 */
};

uint8_t first_AMF[13] = {
    0x02,  /* AMF package type: AMF_DATA_TYPE_STRING */
    0x00, 0x0a,  /* package data len, always to be 10 */
    0x6f, 0x6e,  /* string "on" */
    0x4d, 0x65, 0x74, 0x61,  /* string "Meta" */
    0x44, 0x61, 0x74, 0x61,  /* string "Data" */
};

// encapulate flv header;
int encapulate_flvheader(const char *filename) {
    FILE *fp = fopen(filename, "ab");
    assert(fp != NULL);

    int writelen = fwrite(flv_header, sizeof(uint8_t), sizeof(flv_header), fp);
    assert(writelen == sizeof(flv_header));
    fflush(fp);
    fclose(fp);

    return 0;
}

// TODO(weizhenwei): encapulate 2 AMFs to flv file;
int encapulate_first_tag(const char *filename) {
    FILE *fp = fopen(filename, "ab");
    assert(fp != NULL);
    fclose(fp);

    return 0;
}

// encapulate first tag body;
// in: the heading 00 00 01 in sps and pps is stripped already;
int encapulate_spspps(uint8_t *sps, int sps_len, uint8_t *pps, int pps_len,
        const char *filename, uint32_t ts) {
    // type infomation assertation;
    assert(sps[0] == 0x67);
    assert(pps[0] == 0x68);

    int offset = 0;
    // 16 = 5  bytes flv video header + 5 bytes AVCDecoderConfigurationRecord
    //      + 1 byte numofsequenceset + 2 bytes sps_len
    //      + 2 byte numofpictureset + 2 bytes pps_len
    uint32_t body_len = sps_len + pps_len + 16;  // 16 = 5 + 5 + 1 + 2 + 1 + 2;
    uint32_t total_tag_len = body_len + FLV_TAG_HEADER_SIZE + FLV_PRE_TAG_SIZE;
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * total_tag_len);
    assert(buffer != NULL);

    // fill flv tag header, 11 bytes;
    buffer[offset++] = 0x09;  // tagtype: video;
    buffer[offset++] = (uint8_t)(body_len >> 16);  // data len;
    buffer[offset++] = (uint8_t)(body_len >> 8);   // data len;
    buffer[offset++] = (uint8_t)(body_len);        // data len;
    buffer[offset++] = (uint8_t)(ts >> 16);  // timestamp;
    buffer[offset++] = (uint8_t)(ts >> 8);   // timestamp;
    buffer[offset++] = (uint8_t)(ts);        // timestamp;
    buffer[offset++] = (uint8_t)(ts >> 24);  // timestamp;
    buffer[offset++] = 0x00;  // stream id 0;
    buffer[offset++] = 0x00;  // stream id 0;
    buffer[offset++] = 0x00;  // stream id 0;

    // fill flv video header, 5 bytes;
    buffer[offset++] = 0x17;  // key frame, AVC;
    buffer[offset++] = 0x00;  // AVC sequence header;
    buffer[offset++] = 0x00;  // composition time;
    buffer[offset++] = 0x00;  // composition time;
    buffer[offset++] = 0x00;  // composition time;

    // fill flv video body, AVCDecoderConfigurationRecord, 5 bytes;
    buffer[offset++] = 0x01;    // configuration version;
    buffer[offset++] = sps[1];  // avcprofileindication;
    buffer[offset++] = sps[2];  // profilecompatibilty;
    buffer[offset++] = sps[3];  // avclevelindication;
    buffer[offset++] = 0xff;    // reserved + lengthsizeminusone;

    // fill sps and pps
    buffer[offset++] = 0xe1;  // numofsequenceset;
    buffer[offset++] = (uint8_t)(sps_len >> 8);  // sps_len high 8 bits;
    buffer[offset++] = (uint8_t)(sps_len);       // sps_len low 8 bits;
    memcpy(buffer + offset, sps, sps_len);
    offset += sps_len;
    buffer[offset++] = 0x01;  // numofpictureset;
    buffer[offset++] = (uint8_t)(pps_len >> 8);  // pps_len high 8 bits;
    buffer[offset++] = (uint8_t)(pps_len);       // pps_len low 8 bits;
    memcpy(buffer + offset, pps, pps_len);
    offset += pps_len;

    // fill previous tag size;
    uint32_t prev_tag_size = body_len + FLV_TAG_HEADER_SIZE;
    buffer[offset++] = (uint8_t)(prev_tag_size >> 24);  // prev tag size;
    buffer[offset++] = (uint8_t)(prev_tag_size >> 16);  // prev tag size;
    buffer[offset++] = (uint8_t)(prev_tag_size >> 8);   // prev tag size;
    buffer[offset++] = (uint8_t)(prev_tag_size);        // prev tag size;

    assert(offset == total_tag_len);

    // write to flv file;
    FILE *fp = fopen(filename, "ab");
    assert(fp != NULL);
    int writelen = fwrite(buffer, sizeof(uint8_t), total_tag_len, fp);
    assert(writelen == total_tag_len);
    fflush(fp);
    fclose(fp);
    free(buffer);

    return 0;
}

// encapulate IDR/SLICE nalu;
int encapulate_nalu(uint8_t *nalu, int nalu_len, const char *filename,
        uint32_t ts, int type) {
    int offset = 0;
    // 9 = 5 bytes flv video header + 4 nalu length;
    uint32_t body_len = nalu_len + 9;  // 9 = 5 + 4;
    uint32_t total_tag_len = body_len + FLV_TAG_HEADER_SIZE + FLV_PRE_TAG_SIZE;
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * total_tag_len);
    assert(buffer != NULL);

    // fill flv tag header, 11 bytes;
    buffer[offset++] = 0x09;  // tagtype: video;
    buffer[offset++] = (uint8_t)(body_len >> 16);  // data len;
    buffer[offset++] = (uint8_t)(body_len >> 8);   // data len;
    buffer[offset++] = (uint8_t)(body_len);        // data len;
    buffer[offset++] = (uint8_t)(ts >> 16);  // timestamp;
    buffer[offset++] = (uint8_t)(ts >> 8);   // timestamp;
    buffer[offset++] = (uint8_t)(ts);        // timestamp;
    buffer[offset++] = (uint8_t)(ts >> 24);  // timestamp;
    buffer[offset++] = 0x00;  // stream id 0;
    buffer[offset++] = 0x00;  // stream id 0;
    buffer[offset++] = 0x00;  // stream id 0;

    // fill flv video header, 5 bytes;
    if (type == NAL_SLICE_IDR) {
        buffer[offset++] = 0x17;  // key frame, AVC;
    } else if (type == NAL_SLICE) {
        buffer[offset++] = 0x27;  // key frame, AVC;
    } else {
        dmd_log(LOG_INFO, "impossible to reach here!\n");
        assert(0);
    }
    buffer[offset++] = 0x01;  // AVC NALU unit;
    buffer[offset++] = 0x00;  // composition time;
    buffer[offset++] = 0x00;  // composition time;
    buffer[offset++] = 0x00;  // composition time;

    // fill flv video body;
    buffer[offset++] = (uint8_t)(nalu_len >> 24);  // nalu length;
    buffer[offset++] = (uint8_t)(nalu_len >> 16);  // nalu length;
    buffer[offset++] = (uint8_t)(nalu_len >> 8);   // nalu length;
    buffer[offset++] = (uint8_t)(nalu_len);        // nalu length;
    memcpy(buffer + offset, nalu, nalu_len);
    offset += nalu_len;

    // fill previous tag size;
    uint32_t prev_tag_size = body_len + FLV_TAG_HEADER_SIZE;
    buffer[offset++] = (uint8_t)(prev_tag_size >> 24);  // prev tag size;
    buffer[offset++] = (uint8_t)(prev_tag_size >> 16);  // prev tag size;
    buffer[offset++] = (uint8_t)(prev_tag_size >> 8);   // prev tag size;
    buffer[offset++] = (uint8_t)(prev_tag_size);       // prev tag size;

    assert(offset == total_tag_len);

    // write to flv file;
    FILE *fp = fopen(filename, "ab");
    assert(fp != NULL);
    int writelen = fwrite(buffer, sizeof(uint8_t), total_tag_len, fp);
    assert(writelen == total_tag_len);
    fflush(fp);
    fclose(fp);
    free(buffer);

    return 0;
}


