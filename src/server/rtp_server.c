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
    } // for

    return 0;
}

void rtp_server_running()
{
    dmd_log(LOG_INFO, "in function %s, rtp server is running\n", __func__);

    while (1) {
        dmd_log(LOG_INFO, "in function %s, hehe\n", __func__);
    }
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

