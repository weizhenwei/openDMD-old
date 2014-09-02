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
 * File: http_utils.c
 *
 * Brief: http protocol parser
 *
 * Date: 2014.08.25
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "http_utils.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "global_context.h"
#include "http_response-inl.h"
#include "log.h"

uint64_t request_count = 0;

const char *hellowHTML = "HTTP/1.0 200 ok\r\n"
                                "Server: openDMD-0.01\r\n"
                                "Connection: close\r\n"
                                "Max-Age: 0\r\n"
                                "Expires: 0\r\n"
                                "Cache-Control: no-cache\r\n"
                                "Cache-Control: private\r\n"
                                "Pragma: no-cache\r\n"
                                "Content-type: text/html\r\n\r\n"
                                "<html>\n"
                                "<head>\n"
                                "<title>HelloHTML</title>\n"
                                "</head>"
                                "<body>\n"
                                "Hello, HTML!\n"
                                "</body>\n"
                                "</html>\n";

const char *hellowWorld = "HTTP/1.0 200 ok\r\n"
                                "Server: openDMD-0.01\r\n"
                                "Connection: close\r\n"
                                "Max-Age: 0\r\n"
                                "Expires: 0\r\n"
                                "Cache-Control: no-cache\r\n"
                                "Cache-Control: private\r\n"
                                "Pragma: no-cache\r\n"
                                "Content-type: text/html\r\n\r\n"
                                "<html>\n"
                                "<head>\n"
                                "<title>HelloWorld</title>\n"
                                "</head>"
                                "<body>\n"
                                "Hello, world!\n"
                                "</body>\n"
                                "</html>\n";

const char *hellowChrome = "HTTP/1.0 200 ok\r\n"
                                "Server: openDMD-0.01\r\n"
                                "Connection: close\r\n"
                                "Max-Age: 0\r\n"
                                "Expires: 0\r\n"
                                "Cache-Control: no-cache\r\n"
                                "Cache-Control: private\r\n"
                                "Pragma: no-cache\r\n"
                                "Content-type: text/html\r\n\r\n"
                                "<html>\n"
                                "<head>\n"
                                "<title>Hello Chrome</title>\n"
                                "</head>"
                                "<body>\n"
                                "Hello, chrome web browser!\n"
                                "</body>\n"
                                "</html>\n";


void sendHello(int fd, const char *msg)
{
    char reply[1024];
    int len = strlen(msg);
    sprintf(reply, msg, len);
    reply[len] = '\0';

    dmd_log(LOG_DEBUG, "reply to send:\n%s\n", reply);
    int sendlen = write(fd, reply, len);
    if (sendlen != len) {
        dmd_log(LOG_ERR, "send length not match: expected send length:%d, "
                "actual send length:%d\n", len, (int)sendlen);
    } else {
        dmd_log(LOG_INFO, "send succeed client\n\n");
    }
}

static void send_open_error_response(int client_fd)
{
    // TODO, may reasons file open failure, parse them.
    switch (errno) {
        case EACCES:
            send_forbidden_response(client_fd);
            break;
        case EISDIR:
            send_not_valid_response(client_fd);
            break;
        default:
            send_not_found_response(client_fd);
            break;
    } // switch
    dmd_log(LOG_INFO, "send succeed client\n\n");
}

static int handle_auth(int client_fd, const char *buffer, int buffer_len)
{
    // TODO: encoding auth here.
    char *auth = "dXNlcm5hbWU6cGFzc3dvcmQ="; // for "username:password";
    char *authentication = NULL;

    if ((authentication = strstr(buffer,"Basic")) != NULL) {
        char *end_auth = NULL;
        authentication = authentication + 6;

        if ((end_auth  = strstr(authentication,"\r\n")) != NULL) {
            authentication[end_auth - authentication] = '\0';
        } else {
            send_authentication(client_fd);
            return -1;
        }

        if (strcmp(auth, authentication) != 0) {
            send_authentication(client_fd);
            return -1;
        } else { // authentication ok!
            return 0;
        }
    } else {
        send_authentication(client_fd);
        return -1;
    }

    return 0;
}

int response_url(int client_fd, const char *url)
{
#define BUFFSIZE 1024

    int fd = -1;
    char *webserver_root = global.webserver_root;
    char response_file[PATH_MAX];
    char buffer[BUFFSIZE];
    bzero(buffer, BUFFSIZE);

    if (strcmp(url, "/") == 0
            || strcmp(url, "/index.html") == 0) { // redirect to index.html;
        sprintf(response_file, "%s/index.html", webserver_root);

    } else if (strcmp(url, "/favicon.ico") == 0) { // chrome browser specific
        sprintf(response_file, "%s/favicon.ico", webserver_root);

    } else if (strcmp(url, "/login.html") == 0) {
        sprintf(response_file, "%s/login.html", webserver_root);

    } else { // 404 not fond;
        send_not_found_response(client_fd);
        return 0;
    }

    // TODO: what if response_file is not a regular file?
    //       using fstat to check it later!
    fd = open(response_file, O_RDONLY);
    if (fd == -1) { // response file not found
        send_open_error_response(client_fd);
        return 0;
    }

    // first send http ok header
    send_ok_response_header(client_fd);
    
    // then send http body;
    int readlen = read(fd, buffer, BUFFSIZE - 1);
    if (readlen > 0) {
        dmd_log(LOG_DEBUG, "response body to send:\n");
        buffer[readlen] = '\0';
    }
    while (readlen > 0) {
        int sendlen = send(client_fd, buffer, readlen, 0);
        assert(sendlen == readlen);
        dmd_log(LOG_DEBUG, "%s", buffer);

        readlen = read(fd, buffer, BUFFSIZE - 1);
        if (readlen >= 0) {
            buffer[readlen] = '\0';
        }
    }
    dmd_log(LOG_INFO, "send succeed client\n\n");

    close(fd);
#undef BUFFSIZE

    // send response ok!
    return 0;
}

int parse_http(int client_fd, const char *client_request, int client_len)
{
    int ret = 0;
    char method[10], url[512], protocol[10];
    bzero(method, 10);
    bzero(url, 512);
    bzero(protocol, 10);

    sscanf((const char * restrict)client_request,
            "%9s %511s %9s", method, url, protocol);

    if (strstr(client_request, "\r\n\r\n") == NULL) {
        send_bad_request_response(client_fd);
        dmd_log(LOG_INFO, "send succeed client\n\n");
        return 0;
    }

    // Check Protocol
    if (strcmp(protocol, "HTTP/1.0") != 0
            && strcmp(protocol, "HTTP/1.1") != 0) {
        // TODO: Unsupported HTTP protocol.
        send_bad_request_response(client_fd);
        dmd_log(LOG_ERR, "Bad HTTP Protocol:%s\n", protocol);
        return 0;
    }

    if (strcmp (method, "GET") != 0) {
        send_method_not_implemented_response(client_fd);
        dmd_log(LOG_INFO, "send succeed client\n\n");
        return 0;
    }

    dmd_log(LOG_INFO, "method: %s, url:%s, protocol:%s\n",
            method, url, protocol);


    ret = handle_auth(client_fd, client_request, client_len);
    if (ret == 0) { // authentication ok, do response
        ret = response_url(client_fd, url);
    }

    return ret;
}

int handle_request(int client_fd)
{
#define BUFFSIZE 1024

    char buffer[BUFFSIZE];
    memset(buffer, '\0', BUFFSIZE);

    int readlen = read(client_fd, buffer, BUFFSIZE);
    if (readlen == -1) {
        if ((errno = EAGAIN) || (errno == EWOULDBLOCK)) {
            dmd_log(LOG_DEBUG, "epoll read later\n");
            return 0;
        }
        close(client_fd);
        return -1;
    } else if (readlen == 0) {
        close(client_fd);
        return 0;
    } else {
        if (readlen == BUFFSIZE) {
            // TODO: there maybe more but we didn't read it,
            //       deal this situation later!
            assert(0);
        }
        buffer[readlen] = '\0';
        dmd_log(LOG_INFO, "read from client:\n%s\n", buffer);

        parse_http(client_fd, buffer, readlen);

        // if (request_count % 3 == 0) {
        //     sendHello(client_fd, hellowHTML);
        // } else if (request_count % 3 == 1) {
        //     sendHello(client_fd, hellowWorld);
        // } else {
        //     sendHello(client_fd, hellowChrome);
        // }
        // request_count++;

        close(client_fd); //remember to close client fd!
    }

#undef BUFFSIZE

    return 0;
}

