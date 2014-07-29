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

/*
 * enum nal_unit_type_e
 * {
 *     NAL_UNKNOWN     = 0,
 *     NAL_SLICE       = 1,
 *     NAL_SLICE_DPA   = 2,
 *     NAL_SLICE_DPB   = 3,
 *     NAL_SLICE_DPC   = 4,
 *     NAL_SLICE_IDR   = 5,    // ref_idc != 0 
 *     NAL_SEI         = 6,    // ref_idc == 0
 *     NAL_SPS         = 7,
 *     NAL_PPS         = 8,
 *     NAL_AUD         = 9,
 *     NAL_FILLER      = 12,
 *     // ref_idc == 0 for 6,9,10,11,12
 * };
 *
 */
static void dump_nalu_type(uint8_t typebyte)
{
    int type = typebyte & 0x1f;

    if (type == NAL_SLICE) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_SLICE\n");
    } else if (type == NAL_SLICE_DPA) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_SLICE_DPA\n");
    } else if (type == NAL_SLICE_DPB) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_SLICE_DPB\n");
    } else if (type == NAL_SLICE_DPC) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_SLICE_DPC\n");
    } else if (type == NAL_SLICE_IDR) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_SLICE_IDR\n");
    } else if (type == NAL_SEI) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_SEI\n");
    } else if (type == NAL_SPS) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_SPS\n");
    } else if (type == NAL_PPS) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_PPS\n");
    } else if (type == NAL_AUD) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_AUD\n");
        /*
    } else if (type == 10) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_EOSEQ\n");
    } else if (type == 11) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NALU_TYPE_EOSTREAM\n");
        */
    } else if (type == NAL_FILLER) {
        dmd_log(LOG_DEBUG, "NALU TYPE: NAL_FILLER\n");
    }
}

static void analyze_nalu(x264_nal_t *nal)
{
    uint8_t *nalu = nal->p_payload;
    int length = nal->i_payload;
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

    uint8_t *p = nalu;
    while (*p == 0x00) {
        p++;
    }
    assert(*p == 0x01);
    p++;
    dump_nalu_type(*p);

}

static struct h264_frame *new_h264_frame()
{
    struct h264_frame *hf = NULL;
    hf = (struct h264_frame *)malloc(sizeof(struct h264_frame));
    assert(hf != NULL);

    hf->sps_len = 0;
    hf->sps_payload = 0;
    hf->pps_len = 0;
    hf->pps_payload = 0;

    hf->idr_total = 0;
    hf->idr_list = NULL;

    hf->idr_total_len = 0;
    hf->idr_payload = NULL;

    return hf;
}

static void dump_h264_frame(struct h264_frame *hf)
{
    dmd_log(LOG_DEBUG, "hf->sps_len = %d\n", hf->sps_len);
    dmd_log(LOG_DEBUG, "hf->pps_len = %d\n", hf->pps_len);
    dmd_log(LOG_DEBUG, "hf->idr_total = %d\n", hf->idr_total);
    dmd_log(LOG_DEBUG, "hf->idr_total_len = %d\n", hf->idr_total_len);
}

static void release_h264_frame(struct h264_frame *hf)
{
    if (hf == NULL)
        return;

    if (hf->sps_payload != NULL) {
        free(hf->sps_payload);
    }
    if (hf->pps_payload != NULL) {
        free(hf->pps_payload);
    }

    struct h264_idr_nalu *p = hf->idr_list;
    struct h264_idr_nalu *idr = p;
    while (idr != NULL) {
        idr = p->next;
        free(p->idr_payload);
        free(p);
        p = idr;
    }

    if (hf->idr_payload != NULL) {
        free(hf->idr_payload);
    }

    free(hf);
}

static int pack_nals(const char *h264file, x264_nal_t *nals, int nnal)
{
    x264_nal_t *nal = nals;

    // SPS + PPS + [SEI] + n * IDR;
    // TODO: this may cause problems;
    assert(nnal >= 4);

    // TODO: ts_inc is heuristic;
    int fps = global.x264_fps;
    int ts_inc = 1000 / fps * 2.5;
    dmd_log(LOG_DEBUG, "in function %s, time stamp increment is %d\n",
            __func__, ts_inc);
    ts += ts_inc;

    struct h264_frame *hf = new_h264_frame();

    // first frame is SPS
    assert(nal->i_type == NAL_SPS); // SPS frame;
    analyze_nalu(nal); // dump the nalu infomation;
    uint8_t *payload = nal->p_payload;
    uint8_t *p = payload;
    int len =  nal->i_payload;
    int offset = 0;
    while (*p == 0x00) { // strip heading 0x00 0x00 ... 0x01;
        p++;
        offset++;
    }
    assert(*p == 0x01);
    p++; offset++;
    int sps_len = len - offset;
    uint8_t *sps = (uint8_t *)malloc(sizeof(uint8_t) * sps_len);
    assert(sps != NULL);
    bzero(sps, sps_len);
    memcpy(sps, payload + offset, sps_len);
    hf->sps_len = sps_len;
    hf->sps_payload = sps;
    dmd_log(LOG_DEBUG, "in function %s, SPS frame length:%d, sps_len = %d\n",
           __func__, len, sps_len);


    nal++;
    // second frame is PPS
    assert(nal->i_type == NAL_PPS); // PPS frame;
    analyze_nalu(nal); // dump the nalu infomation;
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
    uint8_t *pps = (uint8_t *)malloc(sizeof(uint8_t) * pps_len);
    assert(pps != NULL);
    bzero(pps, pps_len);
    memcpy(pps, payload + offset, pps_len);
    hf->pps_len = pps_len;
    hf->pps_payload = pps;
    dmd_log(LOG_DEBUG, "in function %s, PPS frame length:%d, pps_len = %d\n",
           __func__, len, pps_len);


    nal++;
    hf->idr_list = (struct h264_idr_nalu *) malloc(sizeof(struct h264_idr_nalu));
    hf->idr_list->idr_len = -1;
    hf->idr_list->idr_payload = NULL;
    hf->idr_list->next = NULL;
    struct h264_idr_nalu *hf_idr = hf->idr_list;
    for (nal = nals + 2; nal < nals + nnal; nal++) {
        if ((nal->i_type != NAL_SLICE_IDR)
                && (nal->i_type != NAL_SLICE)) { // only write IDR/SLICE frame;
            continue;
        }
        assert(nal->i_type == NAL_SLICE_IDR); // IDR frame;
        // dump the nalu infomation;
        analyze_nalu(nal);

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
        uint8_t *idr = (uint8_t *)malloc(sizeof(uint8_t) * idr_len);
        bzero(idr, idr_len);
        memcpy(idr, payload + offset, idr_len);
        struct h264_idr_nalu *idr_nalu = (struct h264_idr_nalu *)
            malloc(sizeof(struct h264_idr_nalu));
        assert(idr_nalu != NULL);
        idr_nalu->idr_len = idr_len;
        idr_nalu->idr_payload = idr;
        idr_nalu->next = NULL;
        hf->idr_total++;
        hf->idr_total_len += idr_len;
        hf_idr->next = idr_nalu;
        hf_idr = hf_idr->next;
        dmd_log(LOG_DEBUG, "in function %s, IDR frame len:%d, idr_len:%d\n",
           __func__, len, idr_len);
    }
    dump_h264_frame(hf);


    // generate idr frame;
    hf->idr_payload = (uint8_t *)
        malloc(sizeof(uint8_t) * hf->idr_total_len);
    hf_idr = hf->idr_list->next;
    unsigned int idx = 0;
    while (hf_idr != NULL) {
        memcpy(hf->idr_payload + idx, hf_idr->idr_payload,
                hf_idr->idr_len);
        idx += hf_idr->idr_len;
        hf_idr = hf_idr->next;
    }
    dmd_log(LOG_DEBUG, "idx = %d, hf->idr_total_len = %d\n",
            idx, hf->idr_total_len);
    assert(idx == hf->idr_total_len);

    // write to flv file
    dmd_log(LOG_DEBUG, "in function %s, before encapulate spspps\n", __func__);
    encapulate_spspps(hf->sps_payload, hf->sps_len,
            hf->pps_payload, hf->pps_len, h264file, ts);

    dmd_log(LOG_DEBUG, "in function %s, before encapulate nalu\n",
            __func__);
    int i_type = NAL_SLICE_IDR;
    encapulate_nalu(hf->idr_payload, hf->idr_total_len,
            h264file, ts, i_type);

    release_h264_frame(hf);

    return 0;
}

// write nalu to flv file and/or network;
static int write_nals_wzw(const char *h264file, x264_nal_t *nals, int nnal)
{
    dmd_log(LOG_DEBUG, "in function %s, at the beginning\n", __func__);
    x264_nal_t *nal = nals;
    enum cluster_mode_t cluster_mode = global.cluster_mode;

    int fps = global.x264_fps;
    int ts_inc = 1000 / fps * 2.5;
    dmd_log(LOG_DEBUG, "in function %s, time stamp increment is %d\n",
            __func__, ts_inc);
    ts += ts_inc;

    // SPS + PPS + SEI + n * IDR;
    assert(nnal >= 4);

    // first frame is SPS
    assert(nal->i_type == NAL_SPS); // SPS frame;
    // dump the nalu infomation;
    analyze_nalu(nal);
    uint8_t *payload = nal->p_payload;
    uint8_t *p = payload;
    int len =  nal->i_payload;
    int offset = 0;
    while (*p == 0x00) { // strip heading 0x00 0x00 ... 0x01;
        p++;
        offset++;
    }
    assert(*p == 0x01);
    p++; offset++;
    int sps_len = len - offset;
    uint8_t *sps = (uint8_t *)
        malloc(sizeof(uint8_t) * sps_len);
    bzero(sps, sps_len);
    memcpy(sps, payload + offset, sps_len);
    dmd_log(LOG_DEBUG, "in function %s, SPS frame length:%d, sps_len = %d\n",
           __func__, len, sps_len);


    nal++;
    // second frame is PPS
    assert(nal->i_type == NAL_PPS); // PPS frame;
    // dump the nalu infomation;
    analyze_nalu(nal);
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
    uint8_t *pps = (uint8_t *)malloc(sizeof(uint8_t) * pps_len);
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
        if ((nal->i_type != NAL_SLICE_IDR)
                && (nal->i_type != NAL_SLICE)) { // only write IDR/SLICE frame;
            continue;
        }
        assert(nal->i_type == NAL_SLICE_IDR); // IDR frame;
        // dump the nalu infomation;
        analyze_nalu(nal);

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
        uint8_t *idr = (uint8_t *)
            malloc(sizeof(uint8_t) * idr_len);
        bzero(idr, idr_len);
        memcpy(idr, payload + offset, idr_len);
        dmd_log(LOG_DEBUG, "in function %s, IDR frame len:%d, idr_len:%d\n",
           __func__, len, idr_len);

        dmd_log(LOG_DEBUG, "in function %s, before encapulate nalu\n",
                __func__);
        encapulate_nalu(idr, idr_len, h264file, ts, nal->i_type);
        free(idr);
    }

    dmd_log(LOG_DEBUG, "in function %s, "
            "end of current H264 frame encapulation\n\n", __func__);

    // if node is client, send h264 frame to server;
    if (cluster_mode == CLUSTER_CLIENT) {
        for (nal = nals; nal < nals + nnal; nal++) {
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
    } // if 

    return 0;
}

// write nalu to flv file and/or network;
static int write_nalss(const char *h264file, x264_nal_t *nals, int nnal)
{
    // int ret = pack_nals(h264file, nals, nnal);
    // assert(ret == 0);
    x264_nal_t *nal = nals;
    enum cluster_mode_t cluster_mode = global.cluster_mode;

    FILE *h264fp = fopen(h264file, "ab");
    assert(h264fp != NULL);
    for (nal = nals; nal < nals + nnal; nal++) {
        int len = fwrite(nal->p_payload, sizeof(uint8_t),
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

    return 0;
}

static int write_nals(const char *h264file, x264_nal_t *nals, int nnal,
        x264_param_t param, x264_picture_t *pic_out)
{
    // global flv_output;
    hnd_t *p_flv = (hnd_t *)malloc(sizeof(hnd_t));
    cli_output_opt_t opt;
    opt.use_dts_compress = 0;
    flv_output.open_file((char *)h264file, p_flv, &opt);
    flv_output.set_param(*p_flv, &param);
    flv_output.write_headers(*p_flv, nals);
    x264_nal_t *nal = nals + 3;
    assert(nal->i_type == NAL_SLICE_IDR);
    flv_output.write_frame(*p_flv, nal->p_payload, nal->i_payload, pic_out);
    flv_output.close_file(*p_flv, 0, 0);

    return 0;
}

static void x264_param_config(x264_param_t *param, int width, int height)
{
    // int fps = 25;  // 25 frames per second;
    int fps = global.x264_fps;
    x264_param_default_preset(param, "veryfast", "zerolatency");

    // basic settings;
    param->i_csp = X264_CSP_I420;
    param->i_threads = 1;
    param->i_width = width;
    param->i_height = height;
    param->i_fps_num = fps;
    param->i_fps_den = 1;
    param->i_log_level = X264_LOG_WARNING;

    // intra refres
    param->i_keyint_max = fps;
    param->b_intra_refresh = 1;
    
    // param->i_slice_max_size = 1400;
    // if (global.cluster_mode == CLUSTER_CLIENT) {
    //     // limit each nalu slice length < 1400, including nalu header,
    //     // so that sending UDP without packet fragmented;
    //     param->i_slice_max_size = 1400;
    // }

    // for rate control;
    // ABR method generate low quality of video;
    // param->rc.i_rc_method = X264_RC_ABR;
    // param->rc.i_bitrate = 256; // bitrate, in kbps(kilobits per second);
    param->rc.i_rc_method = X264_RC_CRF;
    param->rc.f_rf_constant = 25;
    param->rc.f_rf_constant_max = 35;

    // for streaming;
    param->b_repeat_headers = 1;
    param->b_annexb = 1;
    // param->b_repeat_headers = 0;
    // param->b_annexb = 0;

    x264_param_apply_profile(param, "baseline");

}

// encode Planar YUV420P to H264 foramt using libx264
int encode_yuv420p(uint8_t *yuv420p, int width, int height,
        const char *h264file)
{
    x264_t *encoder;
    x264_picture_t pic_in, pic_out;

    // initialize param;
    x264_param_t param;
    x264_param_config(&param, width, height);
    
    // create encoder;
    encoder = x264_encoder_open(&param);
    x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);

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
    write_nals_wzw(h264file, nals, nnal);
    // write_nals(h264file, nals, nnal, param, &pic_out);

    // WARNING: remember to free pic_in;
    x264_picture_clean(&pic_in);
    x264_encoder_close(encoder);

    return 0;
}

