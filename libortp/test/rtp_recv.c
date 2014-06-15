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
 * Brief: ortp receive test;
 *
 * Date: 2014.06.15
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "rtp_recv.h"

void rtp_recv_init()
{
	ortp_init();
	ortp_scheduler_init();
}

void rtp_recv_release()
{
    ortp_exit();
}


RtpSession rtp_recv_createSession(const char *localIP, const int localPort)
{
    RtpSession *rtpsession = rtp_session_new(RTP_SESSION_SENDONLY);	

	rtp_session_set_scheduling_mode(rtpsession, 1);
	rtp_session_set_blocking_mode(rtpsession, 1);
	rtp_session_set_remote_addr(rtpsession, localIP, localPort);
	rtp_session_set_payload_type(rtpsession, PAYLOAD_TYPE_VIDEO);

    return rtpsession;
}

int rtp_send_recvdata(RtpSession rtpsession, char *buffer, int *len)
{
	int recvBytes  = 0;
	int totalBytes = 0;
	int have_more = 1;

#if 0
	while (have_more) {
		if ( totalBytes + READ_RECV_PER_TIME > len ) {
            return false;
		}
		recvBytes = rtp_session_recv_with_ts(rtpsession,
                buffer + totalBytes, READ_RECV_PER_TIME,
                m_curTimeStamp, &have_more);

		if (recvBytes <= 0) {
			break;
		}

		totalBytes += recvBytes;
	}

	if (totalBytes == 0) {
		return false;
	}
	len = totalBytes;

	m_curTimeStamp += m_timeStampInc;
#endif

    return 0;
}

