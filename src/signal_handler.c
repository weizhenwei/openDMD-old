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
 * File: signal_handler.c
 *
 * Brief: signal handler of the project
 *
 * Date: 2014.05.22
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "src/signal_handler.h"

#include <assert.h>
#include <bits/signum.h>
#include <stdlib.h>
#include <signal.h>

#include "src/global_context.h"
#include "src/log.h"

int client_running = 1;
int server_running = 1;

static void sigint_handler(int signal) {
    assert(signal == SIGINT);

    if (global.cluster_mode == CLUSTER_CLIENT
            || global.cluster_mode == CLUSTER_SINGLETON) {
        if (global.client.working_mode == CAPTURE_ALL) {
            // notify picture thread and video thread to exit;
            pthread_mutex_lock(&global.client.thread_attr.picture_mutex);
            global.client.picture_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.picture_cond);
            pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);

            pthread_mutex_lock(&global.client.thread_attr.video_mutex);
            global.client.video_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.video_cond);
            pthread_mutex_unlock(&global.client.thread_attr.video_mutex);

        } else if (global.client.working_mode == CAPTURE_PICTURE) {
            // notify picture thread to exit;
            pthread_mutex_lock(&global.client.thread_attr.picture_mutex);
            global.client.picture_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.picture_cond);
            pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);

        } else if (global.client.working_mode == CAPTURE_VIDEO) {
            // notify video thread to exit;
            pthread_mutex_lock(&global.client.thread_attr.video_mutex);
            global.client.video_target = NOTIFY_EXIT;
            pthread_cond_signal(&global.client.thread_attr.video_cond);
            pthread_mutex_unlock(&global.client.thread_attr.video_mutex);

        } else {
            dmd_log(LOG_ERR, "in function %s, impossible reach here!\n",
                    __func__);
            assert(0);
        }

        // notify main thread to stop
        client_running = 0;
    } else if (global.cluster_mode == CLUSTER_SERVER) {
        server_running = 0;
    } else {
        dmd_log(LOG_ERR, "in function %s, can't reach here!\n",
                __func__);
        assert(0);
    }

    dmd_log(LOG_INFO, "in function %s, "
            "captured SIGINT (Ctrl + C), program exit\n", __func__);
}

static void sighup_handler(int signal) {
    // TODO(weizhenwei): reload config file;
    dmd_log(LOG_INFO, "reload config file\n");
}

void signal_init() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, SIG_DFL);

    // signal Ctrl+C, capture it manually;
    signal(SIGINT, sigint_handler);

    // SIGHUP handler, reload config file;
    signal(SIGHUP, sighup_handler);
}

void signal_register(int sig, void (*sighandler)(int)) {
    // TODO(weizhenwei): fulfill this function later!
    signal(sig, sighandler);
}

