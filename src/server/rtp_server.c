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
 * File: rtp_server.c
 *
 * Brief: ortp server running;
 *
 * Date: 2014.06.22
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "rtp_server.h"


// init rtpsession;
int rtp_server_init()
{
    int i = 0;
    rtp_recv_init();

    // init server repository directory;
    if (server_init_repodir() != 0) {
        return -1;
    }

    char *serverIP = global.server.server_ip;
    int client_scale = global.server.client_scale;
    int server_port_base = global.server.server_port_base;

    global.server.client_items = (struct server_client_item *)malloc(
            sizeof(struct server_client_item) * client_scale);
    if (global.server.client_items == NULL) {
        dmd_log(LOG_ERR, "in function %s, failed to malloc for "
                "server_client_item\n", __func__);
        return -1;
    }

    for (i = 0; i < client_scale; i++) {
        // step1: create rtpsession and init;
        global.server.client_items[i].rtpsession = rtp_recv_createSession(
                serverIP, server_port_base + 2 * (i + 1));
        if (global.server.client_items[i].rtpsession == NULL) {
            dmd_log(LOG_ERR, "in function %s, failed to malloc for "
                    "rtpsession\n", __func__);
            return -1;
        }
        global.server.client_items[i].lasttime = 0;
        global.server.client_items[i].fp = NULL;
        bzero(global.server.client_items[i].filename, PATH_MAX);

        // step2: init store repository directory;
        if (server_init_client_repodir(i + 1) != 0) {
            return -1;
        }
    } // for

    return 0;
}

static int deal_with_client(int i, uint32_t user_ts)
{
    RtpSession *rtpsession = global.server.client_items[i].rtpsession;
    time_t lasttime = global.server.client_items[i].lasttime;
    time_t last_duration = global.server.last_duration;
    FILE *fp = NULL;
    
    dmd_log(LOG_DEBUG, "in function %s, dealing with client %d\n",
            __func__, i + 1);

    unsigned char buffer[RECV_LEN];
	int readlen, havemore = 1;
	while (havemore) {
        dmd_log(LOG_DEBUG, "in function %s, in while loop\n", __func__);

		readlen = rtp_session_recv_with_ts(rtpsession, buffer, RECV_LEN,
                user_ts, &havemore);

        dmd_log(LOG_DEBUG, "in function %s, readlen = %d, havemore=%d\n",
                __func__, readlen, havemore);

		if (readlen > 0) {
            // create an new h264 file;
            time_t now = time(&now);
            assert(now != -1);
            if (now - lasttime > last_duration) {
                char *filename = server_get_filepath(H264_FILE, i+1);
                strncpy(global.server.client_items[i].filename,
                        filename, strlen(filename));
            }

            global.server.client_items[i].lasttime = now;

            char *h264file = global.server.client_items[i].filename;
            assert(h264file != NULL);
            fp = global.server.client_items[i].fp;
            fp = fopen(h264file, "a");
            assert(fp != NULL);

			size_t ret = fwrite(buffer, sizeof(unsigned char), readlen, fp);
			assert( ret == readlen );

            fflush(fp);
            fclose(fp);
		}
	}

	return 0;
}

void rtp_server_running()
{
    SessionSet *set;
    int client_scale = global.server.client_scale;

    dmd_log(LOG_INFO, "in function %s, rtp server is running\n", __func__);

    set = session_set_new();
    while (server_running) {

        int i = 0;
        for (i = 0; i < client_scale; i++) {
            session_set_set(set, global.server.client_items[i].rtpsession);
        }

        i = session_set_select(set, NULL, NULL);
        dmd_log(LOG_INFO, "session_set_select is returning 0 ...\n");

        for (i = 0; i < client_scale; i++) {
            if (session_set_is_set(set,
                        global.server.client_items[i].rtpsession)) {
                dmd_log(LOG_INFO, "dealing with client %d\n", i + 1);
                deal_with_client(i, global.server.user_ts);
                dmd_log(LOG_INFO, "back to function %s\n", __func__);
            }
        } // for

        global.server.user_ts +=  VIDEO_TIME_STAMP_INC;
    } // while
}

// release rtp_session;
void rtp_server_clean()
{
    int i = 0;
    int client_scale = global.server.client_scale;

    for (i = 0; i < client_scale; i++) {
        rtp_recv_release(global.server.client_items[i].rtpsession);
    } // for

    free(global.server.client_items);
    global.server.client_items = NULL;
}

