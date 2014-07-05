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


// global time stamp;
uint32_t ts = 0;

static int dump_nalu_type(unsigned char typebyte)
{
    int type = typebyte & 0x1f;

    if (type == 1) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_SLICE\n");
    } else if (type == 2) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_DPA\n");
    } else if (type == 3) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_DPB\n");
    } else if (type == 4) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_DPC\n");
    } else if (type == 5) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_IDR\n");
    } else if (type == 6) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_SEI\n");
    } else if (type == 7) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_SPS\n");
    } else if (type == 8) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_PPS\n");
    } else if (type == 9) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_AUD\n");
    } else if (type == 10) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_EOSEQ\n");
    } else if (type == 11) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_EOSTREAM\n");
    } else if (type == 12) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_FILL\n");
    }

    return type;
}

static void analyze_nalu(const unsigned char *nalu, int length)
{
    char buffer[30];
    int index = 0;
    assert(length > 4);
    // debug info, dump heading bytes;
    if (length >= 10) {
        int j = 0;
        for (j = 0; j < 10; j++) {
            sprintf(buffer + index, "%02X ", nalu[j]);
            index += 3;
        }

        dmd_log(LOG_DEBUG, "dump first 10 bytes of this NALU: %s\n", buffer);
    } else {
        int j = 0;
        for (j = 0; j < length; j++) {
            sprintf(buffer + index, "%02X ", nalu[j]);
            index += 3;
        }

        dmd_log(LOG_DEBUG, "dump first %d bytes of this NALU: %s\n",
                length, buffer);
    }

    const unsigned char *p = nalu;
    while (*p == 0x00) {
        p++;
    }
    assert(*p == 0x01);
    p++;

    dump_nalu_type(*p);
}

// write nalu to flv file and/or network;
static int write_nals(const char *h264file, x264_nal_t *nals, int nnal)
{
    dmd_log(LOG_DEBUG, "in function %s, at the beginning\n", __func__);
    x264_nal_t *nal = nals;
    // enum cluster_mode_t cluster_mode = global.cluster_mode;

    int fps = global.x264_fps;
    int ts_inc = 1000 / fps * 2;
    dmd_log(LOG_DEBUG, "in function %s, time stamp increment is %d\n",
            __func__, ts_inc);
    ts += ts_inc;

    // SPS + PPS + SEI + n * IDR;
    assert(nnal >= 4);

    // first frame is SPS
    assert(nal->i_type == NAL_SPS); // SPS frame;
    // dump the nalu infomation;
    analyze_nalu(nal->p_payload, nal->i_payload);
    unsigned char *payload = nal->p_payload;
    unsigned char *p = payload;
    int len =  nal->i_payload;
    int offset = 0;
    while (*p == 0x00) { // strip heading 0x00 0x00 ... 0x01;
        p++;
        offset++;
    }
    assert(*p == 0x01);
    p++; offset++;
    int sps_len = len - offset;
    unsigned char *sps = (unsigned char *)
        malloc(sizeof(unsigned char) * sps_len);
    bzero(sps, sps_len);
    memcpy(sps, payload + offset, sps_len);
    dmd_log(LOG_DEBUG, "in function %s, SPS frame length:%d, sps_len = %d\n",
           __func__, len, sps_len);


    nal++;
    // second frame is PPS
    assert(nal->i_type == NAL_PPS); // PPS frame;
    // dump the nalu infomation;
    analyze_nalu(nal->p_payload, nal->i_payload);
    payload = nal->p_payload;
    p = payload;
    len =  nal->i_payload;
    offset = 0;
    while (*p == 0x00) { // strip heading 0x00 0x00 ... 0x01;
        p++;
        offset++;
    }
    assert(*p == 0x01);
    p++; offset++;
    int pps_len = len - offset;
    unsigned char *pps = (unsigned char *)malloc(sizeof(unsigned char) * pps_len);
    bzero(pps, pps_len);
    memcpy(pps, payload + offset, pps_len);
    dmd_log(LOG_DEBUG, "in function %s, PPS frame length:%d, pps_len = %d\n",
           __func__, len, pps_len);

    dmd_log(LOG_DEBUG, "in function %s, before encapulate spspps\n", __func__);
    encapulate_spspps(sps, sps_len, pps, pps_len,  h264file, ts);
    free(sps);
    free(pps);

    nal++;
    for (nal = nals + 2; nal < nals + nnal; nal++) {
        if (nal->i_type != NAL_SLICE_IDR) { // only write IDR frame;
            continue;
        }
        assert(nal->i_type == NAL_SLICE_IDR); // IDR frame;
        // dump the nalu infomation;
        analyze_nalu(nal->p_payload, nal->i_payload);

        payload = nal->p_payload;
        p = payload;
        len =  nal->i_payload;
        offset = 0;
        while (*p == 0x00) { // strip heading 0x00 0x00 ... 0x01;
            p++;
            offset++;
        }
        assert(*p == 0x01);
        p++; offset++;
        int idr_len = len - offset;
        unsigned char *idr = (unsigned char *)
            malloc(sizeof(unsigned char) * idr_len);
        bzero(idr, idr_len);
        memcpy(idr, payload + offset, idr_len);
        dmd_log(LOG_DEBUG, "in function %s, IDR frame len:%d, idr_len:%d\n",
           __func__, len, idr_len);
        
        dmd_log(LOG_DEBUG, "in function %s, before encapulate nalu\n",
                __func__);
        encapulate_nalu(idr, idr_len, h264file, ts);
        free(idr);
    }

    dmd_log(LOG_DEBUG, "in function %s, "
            "end of current H264 frame encapulation\n\n", __func__);

    return 0;

#if 0
        int len = fwrite(nal->p_payload, sizeof(unsigned char),
                nal->i_payload, h264fp);
        dmd_log(LOG_DEBUG, "write to h264 length:%d\n", len);


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
#endif
}

// encode Planar YUV420P to H264 foramt using libx264
int encode_yuv420p(unsigned char *yuv420p, int width, int height,
        const char *h264file)
{
    // int fps = 25;  // 25 frames per second;
    int fps = global.x264_fps;
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
    
    // param.i_slice_max_size = 1400;
    // if (global.cluster_mode == CLUSTER_CLIENT) {
    //     // limit each nalu slice length < 1400, including nalu header,
    //     // so that sending UDP without packet fragmented;
    //     param.i_slice_max_size = 1400;
    // }

    // for rate control;
    // ABR method generate low quality of video;
    // param.rc.i_rc_method = X264_RC_ABR;
    // param.rc.i_bitrate = 256; // bitrate, in kbps(kilobits per second);
    param.rc.i_rc_method = X264_RC_CRF;
    param.rc.f_rf_constant = 25;
    param.rc.f_rf_constant_max = 35;

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

    static int64_t i_pts = 0;
    x264_nal_t *nals;
    int nnal;
    pic_in.i_pts = i_pts++;
    x264_encoder_encode(encoder, &nals, &nnal, &pic_in, &pic_out);

    // write to file or network;
    write_nals(h264file, nals, nnal);

    // WARNING: remember to free pic_in;
    x264_picture_clean(&pic_in);
    x264_encoder_close(encoder);

    return 0;
}

