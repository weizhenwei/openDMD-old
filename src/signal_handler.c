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

#include "signal_handler.h"

static void sigint_handler(int signal)
{
    assert(signal == SIGINT);

    if (global.cluster_mode == CLUSTER_CLIENT
            || global.cluster_mode == CLUSTER_SINGLETON) {
        // notify picture thread and video thread to exit;
        pthread_mutex_lock(&global.client.thread_attr.picture_mutex);
        global.client.picture_target = NOTIFY_EXIT;
        pthread_cond_signal(&global.client.thread_attr.picture_cond);
        pthread_mutex_unlock(&global.client.thread_attr.picture_mutex);

        pthread_mutex_lock(&global.client.thread_attr.video_mutex);
        global.client.video_target = NOTIFY_EXIT;
        pthread_cond_signal(&global.client.thread_attr.video_cond);
        pthread_mutex_unlock(&global.client.thread_attr.video_mutex);
    }

    dmd_log(LOG_INFO, "captured SIGINT (Ctrl + C), program exit\n");
    exit(EXIT_FAILURE);
}

void signal_init()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_DFL);
    signal(SIGTERM, SIG_DFL);

    // signal Ctrl+C, capture it manually;
    signal(SIGINT, sigint_handler);
}

void signal_register(int sig, void (*sighandler)(int))
{
    //TODO
    signal(sig, sighandler);
}
