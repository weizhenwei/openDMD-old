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
 * File: socket_utils.c
 *
 * Brief: socket operation for webserver
 *
 * Date: 2014.08.25
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "socket_utils.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "global_context.h"
#include "http_utils.h"
#include "log.h"


struct sockaddr *webserver_serverAddr = NULL;
struct sockaddr *webserver_clientAddr = NULL;

int newSocket(void)
{
    int reuse = 1;
    int sendbuffer = SENDBUF;
    int recvbuffer = RECVBUF;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        dmd_log(LOG_ERR, "can not create socket:%s\n", strerror(errno));
        return -1;
    }

    // set sockfd to reuse address, and set its send && recv buffer
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuffer, sizeof(sendbuffer));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuffer, sizeof(recvbuffer));

    // set fd to nonblocking
    int old = fcntl(sockfd, F_GETFL);
    int new = old | O_NONBLOCK;
    fcntl(sockfd, F_SETFL, new);

    return sockfd;
}

void closeSocket(int sockfd)
{
    close(sockfd);
}

struct sockaddr *newAddress()
{
    struct sockaddr_in *addr =
        (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    if (addr == NULL) {
        dmd_log(LOG_ERR, "malloc struct sockaddr_in error!\n");
        return NULL;
    }

    bzero(addr, sizeof(struct sockaddr_in));
    //in_addr_t bind_addr = inet_addr(BIND_ADDR);
    //network byte order
    addr->sin_family = AF_INET;
    addr->sin_port = htons(global.webserver_port);
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    // addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    // addr->sin_addr.s_addr = htonl(global.webserver_ip);
    // TODO: why this code doesn't work ?
    // inet_pton(AF_INET, global.webserver_ip, &addr->sin_addr);

    return (struct sockaddr *) addr;
}

void releaseAddress(struct sockaddr *addr)
{
    if (addr) {
        free(addr);
    }
}

int bindAddress(int sockfd, struct sockaddr *addr)
{
    int ret = bind(sockfd, addr, sizeof(struct sockaddr));
    if (ret == -1) {
        dmd_log(LOG_ERR, "bind socket addr error:%s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int listenAddress(int sockfd)
{
    int ret = listen(sockfd, LISTEN_BACKLOG);
    if (ret == -1) {
        dmd_log(LOG_ERR, "listen socket error:%s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int acceptConnection(int sockfd, struct sockaddr *clientAddress)
{
    int addrlen = sizeof(*clientAddress);
    int clientfd = accept(sockfd, clientAddress,
            (socklen_t * restrict)&addrlen);
    if (clientfd == -1) {
        dmd_log(LOG_ERR, "accept client socket error:%s\n", strerror(errno));
        return -1;
    }

    return clientfd;
}

int newEpollSocket(void)
{
    int epollfd = epoll_create(5);

    if (epollfd == -1) {
        dmd_log(LOG_ERR, "create epoll socket error:%s\n", strerror(errno));
        return -1;
    }

    return epollfd;
}

int addSockfd(int epollfd, int fd)
{
    struct epoll_event event;
    // WARNING: if we don't empty struct event, 
    //          valgrind will report an error of uninitialised byte.
    bzero(&event, sizeof(event)); 

    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET; //Read and Edge Trigger

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);

    return 0;
}

void handleEvent(int epollfd, int sockfd, struct epoll_event *events,
        int nevents)
{
    int i = 0;
    for (i = 0; i < nevents; i++) {
        int fd = events[i].data.fd;

        if(fd == sockfd) { //new client arrival

            dmd_log(LOG_DEBUG, "in function %s, new client arrival\n",
                    __func__);

            struct sockaddr_in *clientAddr = (struct sockaddr_in *)
                malloc(sizeof(struct sockaddr_in));
            if (clientAddr == NULL) {
                dmd_log(LOG_ERR, "in function %s, "
                        "malloc client address error!\n", __func__);
                return;
            }
            bzero(clientAddr, sizeof(*clientAddr));

            int clientfd = acceptConnection(sockfd,
                    (struct sockaddr *)clientAddr);
            addSockfd(epollfd, clientfd);
            free(clientAddr);
        } else if (events[i].events & EPOLLIN) { // read events happened
            dmd_log(LOG_DEBUG, "EPOLLIN happened\n");

            handle_request(fd);
        } else {
            dmd_log(LOG_ERR, "something unknown happend!\n");
        }
    } // for

}

