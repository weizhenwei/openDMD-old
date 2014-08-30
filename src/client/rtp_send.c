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
 * File: rtp_send.c
 *
 * Brief: ortp send captured h264 video;
 *
 * Date: 2014.06.20
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "rtp_send.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <ortp/ortp.h>

#include "global_context.h"
#include "log.h"

void rtp_send_init()
{
	ortp_init();
	ortp_scheduler_init();
    ortp_set_log_level_mask(ORTP_DEBUG | ORTP_MESSAGE
            | ORTP_WARNING | ORTP_ERROR);

}

void rtp_send_release(RtpSession *rtpsession)
{
    ortp_exit();
    rtp_session_destroy(rtpsession);
    ortp_global_stats_display();
}


RtpSession *rtp_send_createSession(
        const char *localIP, const int localPort,
        const char *remoteIP, const int remotePort)
{
    RtpSession *rtpsession = rtp_session_new(RTP_SESSION_SENDONLY);	
    assert(rtpsession != NULL);

	rtp_session_set_scheduling_mode(rtpsession, 1);
    // WARNING: in multiple receiving condtion, block mode is must unset;
	rtp_session_set_blocking_mode(rtpsession, 0);
    rtp_session_set_connected_mode(rtpsession, 1); // 1 means TRUE;
	rtp_session_set_local_addr(rtpsession, localIP, localPort, -1);
	rtp_session_set_remote_addr(rtpsession, remoteIP, remotePort);

    rtp_session_set_symmetric_rtp(rtpsession, 1); // 1 means TRUE;

    // set payload_type to H264 (96);
	rtp_session_set_payload_type(rtpsession, PAYLOAD_TYPE_H264);

	char *ssrc = getenv("SSRC");
	if (ssrc != NULL) {
		rtp_session_set_ssrc(rtpsession, atoi(ssrc));
	}

    return rtpsession;
}
    
int rtp_send(RtpSession *rtpsession, unsigned char *buffer, int len)
{
    int payloadlen = 0;
    int sendlen = 0;
    int remainlen = len;
    int idx = 0;
    int totalsendlen = 0;

    dmd_log(LOG_INFO, "in function %s, send buffer len = %d\n", __func__, len);

    while (remainlen > 0) {
        if (remainlen < SEND_LEN) {
            sendlen = rtp_session_send_with_ts(rtpsession, buffer + idx,
                    remainlen, global.client.clientrtp.user_ts);
            payloadlen = remainlen;
        } else {
            sendlen = rtp_session_send_with_ts(rtpsession, buffer + idx,
                    SEND_LEN, global.client.clientrtp.user_ts);
            payloadlen = SEND_LEN;
        }
        dmd_log(LOG_INFO, "in function %s, send total len = %d, "
                "send payload len = %d\n", __func__, sendlen, payloadlen);

        totalsendlen += payloadlen;
        remainlen -= payloadlen;
        idx += payloadlen;
        global.client.clientrtp.user_ts += VIDEO_TIME_STAMP_INC;
    }

    // Be sure send successfully!
    assert(totalsendlen == len);

    return 0;
}

