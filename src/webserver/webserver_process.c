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
 * File: webserver_process.c
 *
 * Brief: webserver process interface for opendmd
 *
 * Date: 2014.08.26
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "webserver_process.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "global_context.h"
#include "http_utils.h"
#include "log.h"
#include "socket_utils.h"


int webserver_fork()
{
    pid_t pid;

	pid = fork();

	if (pid < 0) {
		dmd_log(LOG_ERR, "webserver process: fork error(%s)",
                strerror(errno));
		return -1;
	} else if (pid > 0) { // parent, just return;
		global.webserver_pid = pid;
		dmd_log(LOG_INFO, "Starting webserver process, pid = %d", pid);

        if (global.client.working_mode == WEBSERVER_ONLY) {
            dmd_log(LOG_INFO, "working_mode = webserver, main process exit.\n");
            exit(EXIT_SUCCESS);
        }

		return 0;
	}

    webserver_loop();

    // Obviously, after main loop, webserver process exit directly.
    exit(EXIT_SUCCESS);
}

void webserver_loop()
{
    dmd_log(LOG_INFO, "in function %s, starting webserver main loop.\n",
            __func__);

    int serverfd = newSocket();
    webserver_serverAddr = newAddress();
    bindAddress(serverfd, webserver_serverAddr);
    listenAddress(serverfd);

    struct epoll_event events[MAX_EPOLL_EVENT];
    int epollfd = newEpollSocket();

    dmd_log(LOG_INFO, "in function %s, begin to work\n", __func__);
    addSockfd(epollfd, serverfd);
    while (1) {
        int ret = epoll_wait(epollfd, events, MAX_EPOLL_EVENT, -1);
        dmd_log(LOG_INFO, "in function %s, after epoll wait\n", __func__);
        if (ret < 0) {
            dmd_log(LOG_ERR, "in function %s, epoll failure\n", __func__);
        } else {
            handleEvent(epollfd, serverfd, events, ret);
        }
    } // while

    closeSocket(serverfd);
    releaseAddress(webserver_serverAddr);

}

