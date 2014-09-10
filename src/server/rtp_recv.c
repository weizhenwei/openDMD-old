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
 * File: rtp_recv.c
 *
 * Brief: ortp receive;
 *
 * Date: 2014.06.21
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "src/server/rtp_recv.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/global_context.h"
#include "src/log.h"

void rtp_recv_init() {
    ortp_init();
    ortp_scheduler_init();
    ortp_set_log_level_mask(ORTP_DEBUG | ORTP_MESSAGE
            | ORTP_WARNING | ORTP_ERROR);
}

void rtp_recv_release(RtpSession *rtpsession) {
    ortp_exit();
    rtp_session_destroy(rtpsession);
    ortp_global_stats_display();
}

RtpSession *rtp_recv_createSession(const char *localIP, const int localPort) {
    RtpSession *rtpsession = rtp_session_new(RTP_SESSION_RECVONLY);
    assert(rtpsession != NULL);

    rtp_session_set_scheduling_mode(rtpsession, 1);
    // WARNING: in multiple receiving condtion, block mode is must unset;
    rtp_session_set_blocking_mode(rtpsession, 0);
    rtp_session_set_local_addr(rtpsession, localIP, localPort, localPort + 1);
    rtp_session_set_connected_mode(rtpsession, 1);  // 1 means TRUE;
    rtp_session_set_symmetric_rtp(rtpsession, 1);  // 1 means TRUE;

    rtp_session_enable_adaptive_jitter_compensation(rtpsession, 1);
    rtp_session_set_jitter_compensation(rtpsession, 40);

    // set video payload_type to H264
    rtp_session_set_payload_type(rtpsession, PAYLOAD_TYPE_H264);

    return rtpsession;
}

static int cond = 1;

void stop_handler(int signum) {
    cond = 0;
}

int rtp_recv(RtpSession *rtpsession, uint32_t *user_ts, const char *recvfile) {
    int recvBytes  = 0;
    int writelen = 0;
    int have_more = 1;
    int stream_received = 0;
    unsigned char buffer[RECV_LEN];

    signal(SIGINT, stop_handler);

    assert(recvfile != NULL);
    FILE *fp = fopen(recvfile, "a");
    assert(fp != NULL);

    while (cond) {
        have_more = 1;
        while (have_more) {
            dmd_log(LOG_DEBUG, "in recv while loop\n");

            recvBytes = rtp_session_recv_with_ts(rtpsession,
                    buffer, RECV_LEN, *user_ts, &have_more);

            if (recvBytes > 0)
                stream_received = 1;

            if ((stream_received) && (recvBytes > 0)) {
                writelen = fwrite(buffer,
                        sizeof(unsigned char), recvBytes, fp);

                dmd_log(LOG_DEBUG, "receive %d bytes, write %d bytes\n",
                        recvBytes, writelen);

                recvBytes = 0;
                writelen = 0;
            }
        }

        *user_ts += VIDEO_TIME_STAMP_INC;
    }

    return 0;
}
