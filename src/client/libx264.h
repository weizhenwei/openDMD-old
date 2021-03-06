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
 * File: libx264.h
 *
 * Brief: encode video to h264 format, using libx264.
 *
 * Date: 2014.05.28
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef SRC_CLIENT_LIBX264_H_
#define SRC_CLIENT_LIBX264_H_

#include <stdint.h>

// global time stamp;
extern uint32_t ts;

// NAL_SLICE_IDR nalu
struct h264_idr_nalu {
    struct h264_idr_nalu *next;
    unsigned int idr_len;  // pure payload len;
    uint8_t *idr_payload;  // idr unit stripped heading 0x00 0x00 0x01
};

// nalu lists of a h264 frame
struct h264_frame {
    unsigned int sps_len;
    uint8_t *sps_payload;
    unsigned int pps_len;
    uint8_t *pps_payload;

    unsigned int idr_total;  // total node of h264_idr_nalu
    struct h264_idr_nalu *idr_list;

    unsigned int idr_total_len;
    uint8_t *idr_payload;
};

// encode Planar YUV420P to H264 foramt using libx264
extern int encode_yuv420p(uint8_t *yuv420p,
        int height, int width, const char *h264file);

#endif  // SRC_CLIENT_LIBX264_H_
