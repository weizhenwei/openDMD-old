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
 * File: libx264.c
 *
 * Brief: encode video to h264 format, using libx264.
 *
 * Date: 2014.05.28
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */
#include "libx264.h"

static void dump_nalu_type(unsigned char typebyte)
{
    int type = typebyte & 0x1f;
    dmd_log(LOG_INFO, "NALU TYPE:");
    if (type == 1) {
        printf("NALU_TYPE_SLICE");
    } else if (type == 2) {
        printf("NALU_TYPE_DPA");
    } else if (type == 3) {
        printf("NALU_TYPE_DPB");
    } else if (type == 4) {
        printf("NALU_TYPE_DPC");
    } else if (type == 5) {
        printf("NALU_TYPE_IDR");
    } else if (type == 6) {
        printf("NALU_TYPE_SEI");
    } else if (type == 7) {
        printf("NALU_TYPE_SPS");
    } else if (type == 8) {
        printf("NALU_TYPE_PPS");
    } else if (type == 9) {
        printf("NALU_TYPE_AUD");
    } else if (type == 10) {
        printf("NALU_TYPE_EOSEQ");
    } else if (type == 11) {
        printf("NALU_TYPE_EOSTREAM");
    } else if (type == 12) {
        printf("NALU_TYPE_FILL");
    }

    printf("\n");
}

static void analyze_nalu(const unsigned char *nalu, int length)
{
    assert(length > 4);
    // debug info, dump heading bytes;
    if (length >= 10) {
        dmd_log(LOG_DEBUG, "dump first 10 bytes of this NALU: ");
        int j = 0;
        for (j = 0; j < 10; j++) {
            printf("%02X ", nalu[j]);
        }
        printf("\n");
    } else {
        dmd_log(LOG_DEBUG, "dump first %d bytes of this NALU: ", length);
        int j = 0;
        for (j = 0; j < length; j++) {
            printf("%02X ", nalu[j]);
        }
        printf("\n");
    }

    const unsigned char *p = nalu;
    while (*p == 0) {
        p++;
    }
    assert(*p == 1);
    p++;

    dump_nalu_type(*p);

}

// encode Planar YUV420P to H264 foramt using libx264
int encode_yuv420p(unsigned char *yuv420p, int width, int height,
        const char *h264file)
{
    int fps = 25;  // 25 frames per second;
    x264_t *encoder;
    x264_picture_t pic_in, pic_out;

    // initialize param;
    x264_param_t param;
    x264_param_default_preset(&param, "veryfast", "zerolatency");
    
    // basic settings;
    param.i_csp = X264_CSP_I420;
    param.i_threads = 1;
    param.i_width = width;
    param.i_height = height;
    param.i_fps_num = fps;
    param.i_fps_den = 1;
    param.i_log_level = X264_LOG_WARNING;


    // intra refres
    param.i_keyint_max = fps;
    param.b_intra_refresh = 1;
    
    param.i_slice_max_size = 1400;
    // if (global.cluster_mode == CLUSTER_CLIENT) {
    //     // limit each nalu slice length < 1400, including nalu header,
    //     // so that sending UDP without packet fragmented;
    //     param.i_slice_max_size = 1400;
    // }

    // for rate control;
    param.rc.i_rc_method = X264_RC_ABR;
    param.rc.i_bitrate = 256; // bitrate, in kbps(kilobits per second);
    // param.rc.i_rc_method = X264_RC_CRF;
    // param.rc.f_rf_constant = 25;
    // param.rc.f_rf_constant_max = 35;

    // for streaming;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;

    x264_param_apply_profile(&param, "baseline");


    // create encoder;
    encoder = x264_encoder_open(&param);

    x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);

    // WARNING: this caused a memory leak!
    // pic_in.img.plane[0] = yuv420p;

    // yuv420p in YUV420P format;
    memcpy(pic_in.img.plane[0], yuv420p, width * height * 1.5);
    pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
    pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;

    assert(h264file != NULL);
    static int64_t i_pts = 0;
    x264_nal_t *nals;
    int nnal;
    pic_in.i_pts = i_pts++;
    x264_encoder_encode(encoder, &nals, &nnal, &pic_in, &pic_out);
    x264_nal_t *nal;
    FILE *h264fp = NULL;
    if ((h264fp = fopen(h264file, "ab+")) == NULL) {
        dmd_log(LOG_ERR, "fopen h264 path error:%s\n", strerror(errno));
        return -1;
    }

    enum cluster_mode_t cluster_mode = global.cluster_mode;
    for (nal = nals; nal < nals + nnal; nal++) {
        int len = fwrite(nal->p_payload, sizeof(unsigned char),
                nal->i_payload, h264fp);
        dmd_log(LOG_DEBUG, "write to h264 length:%d\n", len);

        // dump the nalu infomation;
        analyze_nalu(nal->p_payload, nal->i_payload);

        // send h264 frame to server;
        if (cluster_mode == CLUSTER_CLIENT) {
            int ret = rtp_send(global.client.clientrtp.rtpsession,
                    nal->p_payload, nal->i_payload);
            
            if (ret == 0) {
                dmd_log(LOG_INFO, "in function %s, send h264 frame to server, "
                        "len = %d\n", __func__, nal->i_payload);
            } else {
                dmd_log(LOG_INFO, "in function %s, failed to send h264 frame "
                        "to server\n", __func__);
            }
        }

        if ( len != nal->i_payload) {
            dmd_log(LOG_ERR, "write to h264 error:%s\n", strerror(errno));
            return -1;
        }
    }
    fclose(h264fp);

    // WARNING: remember to free pic_in;
    x264_picture_clean(&pic_in);
    x264_encoder_close(encoder);

    return 0;
}

