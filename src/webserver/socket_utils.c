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

int newSocket(void)
{
    int reuse = 1;
    int sendbuffer = SENDBUF;
    int recvbuffer = RECVBUF;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("can not create socket:");
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
        printf("malloc struct sockaddr_in error!\n");
        return NULL;
    }

    bzero(addr, sizeof(struct sockaddr_in));
    //in_addr_t bind_addr = inet_addr(BIND_ADDR);
    //network byte order
    addr->sin_family = AF_INET;
    addr->sin_port = htons(BIND_PORT);
    addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    return (struct sockaddr *) addr;
}

void releaseAddress(struct sockaddr *addr)
{
    if (addr) {
        free(addr);
    } else {
        printf("Error: An NULL address to free!\n");
    }
}

int bindAddress(int sockfd, struct sockaddr *addr)
{
    int ret = bind(sockfd, addr, sizeof(struct sockaddr));
    if (ret == -1) {
        perror("bind sock addr error:");
        return -1;
    }

    return 0;
}

int listenAddress(int sockfd)
{
    int ret = listen(sockfd, LISTEN_BACKLOG);
    if (ret == -1) {
        perror("listen sock error:");
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
        perror("accept client sock error:");
        return -1;
    }

    return clientfd;
}

int newEpollSocket(void)
{
    int epollfd = epoll_create(5);

    if (epollfd == -1) {
        perror("can not create epoll socket:");
        return -1;
    }

    return epollfd;
}

int addSockfd(int epollfd, int fd)
{
    struct epoll_event event;
    // if we don't empty struct event, 
    // valgrind will report an error of uninitialised byte.
    bzero(&event, sizeof(event)); 

    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET; //Read and Edge Trigger

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);

    return 0;
}

void handleEvent(int epollfd, int sockfd, struct epoll_event *events,
        int nevents, int *count)
{
    char buffer[BUFFSIZE];
    int i = 0;

    for (i = 0; i < nevents; i++) {
        int fd = events[i].data.fd;

        if(fd == sockfd) { //new client arrival

            printf("new client arrival\n");

            struct sockaddr_in *clientAddr = (struct sockaddr_in *)
                malloc(sizeof(struct sockaddr_in));
            if (clientAddr == NULL) {
                printf("malloc client address error!\n");
                return;
            }
            bzero(clientAddr, sizeof(*clientAddr));

            int clientfd = acceptConnection(sockfd,
                    (struct sockaddr *)clientAddr);
            addSockfd(epollfd, clientfd);
            free(clientAddr);
        } else if (events[i].events & EPOLLIN) { // read events happened

            printf("EPOLLIN happened\n");
            while (1) {
                memset(buffer, '\0', BUFFSIZE);
                int ret = recv(fd, buffer, BUFFSIZE, 0);
                if (ret == -1) {
                    if ((errno = EAGAIN) || (errno == EWOULDBLOCK)) {
                        printf("epoll read later\n");
                        break;
                    }
                    closeSocket(fd);
                    break;
                } else if (ret == 0) {
                    closeSocket(fd);
                } else {
                    printf("read from client:\n%s\n", buffer);

                    if (*count % 2 == 0) {
                        sendHello(fd, hellowHTML);
                    } else {
                        sendHello(fd, hellowWorld);
                    }

                    (*count)++;
                    closeSocket(fd); //remember to close client fd!
                }

            } // while
        } else {
            printf("something unknown happend!\n");

        }
    } // for

}

int mainLoop()
{
    syslog(LOG_INFO, "Starting main loop.");

    int serverfd = newSocket();
    serverAddr = newAddress();
    bindAddress(serverfd, serverAddr);
    listenAddress(serverfd);

    struct epoll_event events[MAX_EPOLL_EVENT];
    int epollfd = newEpollSocket();

    printf("begin to work\n");
    int count = 0;
    addSockfd(epollfd, serverfd);
    while (1) {
        int ret = epoll_wait(epollfd, events, MAX_EPOLL_EVENT, -1);
        printf("after epoll wait\n");
        if (ret < 0) {
            printf("epoll failure\n");
        } else {
            handleEvent(epollfd, serverfd, events, ret, &count);
        }
    } // while

    closeSocket(serverfd);
    releaseAddress(serverAddr);
}

