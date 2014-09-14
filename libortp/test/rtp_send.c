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
 * File: rtp_send.h
 *
 * Brief: ortp send test;
 *
 * Date: 2014.06.15
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "libortp/test/rtp_send.h"

void rtp_send_init() {
    ortp_init();
    ortp_scheduler_init();
    ortp_set_log_level_mask(ORTP_DEBUG | ORTP_MESSAGE
            | ORTP_WARNING | ORTP_ERROR);
    // ortp_set_log_level_mask(ORTP_ERROR);
}

void rtp_send_release() {
    ortp_exit();
}

RtpSession *rtp_send_createSession(
        const char *clientIP, const int clientPort,
        const char *remoteIP, const int remotePort) {
    RtpSession *rtpsession = rtp_session_new(RTP_SESSION_SENDONLY);
    assert(rtpsession != NULL);

    rtp_session_set_scheduling_mode(rtpsession, 1);
    rtp_session_set_blocking_mode(rtpsession, 0);
    rtp_session_set_connected_mode(rtpsession, 1);  // 1 means TRUE;
    // rtp_session_set_local_addr(rtpsession,
    // clientIP, clientPort, clientPort+1);
    rtp_session_set_remote_addr(rtpsession, remoteIP, remotePort);

    rtp_session_set_symmetric_rtp(rtpsession, 1);

    rtp_session_set_source_description(
            rtpsession,
            "cname",  /*cname*/
            "name",   /*name*/
            "weizhenwei1988@gmail.com",  /*email*/
            "110",   /*phone number*/
            "loc",   /*loc*/
            "tool",  /*tool*/
            "note:rtp_send test");

    rtp_session_enable_rtcp(rtpsession, 1);  // 1 means TRUE;

    // set payload type to H264 (96);
    rtp_session_set_payload_type(rtpsession, PAYLOAD_TYPE_H264);

    char *ssrc = getenv("SSRC");
    if (ssrc != NULL) {
        rtp_session_set_ssrc(rtpsession, atoi(ssrc));
    }

    return rtpsession;
}

void rtp_send(const char *sendfile,
        const char *clientIP, const int clientPort,
        const char *remoteIP, const int remotePort) {
    unsigned char buffer[SEND_LEN];
    unsigned int user_ts = 0;
    int readlen = 0;
    int sendlen = 0;

    rtp_send_init();
    RtpSession *rtpsession = rtp_send_createSession(
            clientIP, clientPort, remoteIP, remotePort);
    assert(rtpsession != NULL);

    assert(sendfile != NULL);
    FILE *fp = fopen(sendfile, "r");
    assert(fp != NULL);

    while ((readlen = fread(buffer,
                    sizeof(unsigned char), SEND_LEN, fp)) > 0) {
        sendlen = rtp_session_send_with_ts(rtpsession,
                buffer, readlen, user_ts);
        printf("read %d bytes, send %d bytes\n", readlen, sendlen);

        user_ts += VIDEO_TIME_STAMP_INC;
    }

    fclose(fp);

    rtp_session_destroy(rtpsession);
    rtp_send_release();
    ortp_global_stats_display();
}

