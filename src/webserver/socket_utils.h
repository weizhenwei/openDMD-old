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
 * File: socket_utils.h
 *
 * Brief: socket operation for webserver
 *
 * Date: 2014.08.25
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <stdint.h>

struct epoll_event;

#define BIND_PORT 8082
#define BIND_ADDR "127.0.0.1"

#define LISTEN_BACKLOG 10
#define SENDBUF 32768
#define RECVBUF 32768

#define MAX_EPOLL_EVENT 1024

#define BUFFSIZE 1024

extern uint64_t request_count;

extern struct sockaddr *webserver_serverAddr;
extern struct sockaddr *webserver_clientAddr;

extern int newSocket();
extern void closeSocket(int sockfd);

extern struct sockaddr *newAddress();
extern void releaseAddress(struct sockaddr *addr);
extern int bindAddress(int sockfd, struct sockaddr *addr);
extern int listenAddress(int sockfd);

extern int acceptConnection(int sockfd, struct sockaddr *clientAddress);

extern int newEpollSocket();
extern int addSockfd(int epollfd, int fd);
extern void handleEvent(int epollfd, int sockfd, struct epoll_event *events,
        int nevents);

#endif
