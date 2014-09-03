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

/* How many bytes it will take to store LEN bytes in base64.  */
#define BASE64_LENGTH(len) (4 * (((len) + 2) / 3))

// Encode the string S of length LENGTH to base64 format and place it
// to STORE.  STORE will be 0-terminated, and must point to a writable
// buffer of at least 1+BASE64_LENGTH(length) bytes.  
// this function originate from motion-3.2.12 softare, netcam_wget.c
static void base64_encode(const char *s, char *store, int length)
{
    /* Conversion table.  */
    static const char tbl[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };
    
    int i;
    unsigned char *p = (unsigned char *)store;

    /* Transform the 3x8 bits to 4x6 bits, as required by base64.  */
    for (i = 0; i < length; i += 3) {
        *p++ = tbl[s[0] >> 2];
        *p++ = tbl[((s[0] & 3) << 4) + (s[1] >> 4)];
        *p++ = tbl[((s[1] & 0xf) << 2) + (s[2] >> 6)];
        *p++ = tbl[s[2] & 0x3f];
        s += 3;
    }
    
    /* Pad the result if necessary...  */
    if (i == length + 1)
        *(p - 1) = '=';
    else if (i == length + 2)
        *(p - 1) = *(p - 2) = '=';

    /* ...and zero-terminate it.  */
    *p = '\0';
}

static int check_auth(int client_fd, const char *client_auth)
{
    const char *userpass = global.webserver_userpass;
    char auth[1024];
    size_t auth_size = strlen(userpass);
    base64_encode(userpass, auth, auth_size);

    if (strcmp(auth, client_auth) == 0) { // ok;
        return 0;
    } else {
        send_authentication(client_fd);
        return -1;
    }

    return 0;
}

int response_url(int client_fd, const char *url, const char *auth)
{
#define BUFFSIZE 1024

    int fd = -1;
    int ret = -1;
    char *webserver_root = global.webserver_root;
    char response_file[PATH_MAX];
    char buffer[BUFFSIZE];
    bzero(buffer, BUFFSIZE);
    int auth_checked = -1;

    if (auth != NULL) {
        ret = check_auth(client_fd, auth);
        if (ret == -1) {
            return 0;
        } else {
            auth_checked = 1;
        }
    }

    if (strcmp(url, "/") == 0 /* redirect to index.html */
            || strcmp(url, "/index.html") == 0) {
        // no need to do the authentication;
        sprintf(response_file, "%s/index.html", webserver_root);
    } else if (strcmp(url, "/favicon.ico") == 0) {
        // no need to do the authentication;
        sprintf(response_file, "%s/favicon.ico", webserver_root);
    } else { // need to do the authentication;
        if (auth_checked == 1) {
            sprintf(response_file, "%s%s", webserver_root, url);
        } else {
            send_authentication(client_fd);
            return 0;
        }
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

    // get authentication info, if any;
    char *auth_start = NULL;
    char *auth_end = NULL;
    char *auth = NULL;
    int auth_len = 0;
    if ((auth_start = strstr(client_request, "Basic")) != NULL) {
        auth_start = auth_start + 6;
        if ((auth_end  = strstr(auth_start, "\r\n")) != NULL) {
            auth_len = auth_end - auth_start;
            auth = (char *)malloc(sizeof(char) * (auth_len + 1));
            strncpy(auth, auth_start, auth_len);
            auth[auth_len] = '\0';
        }
    }

    dmd_log(LOG_INFO, "method: %s, url:%s, protocol:%s\n",
            method, url, protocol);
    
    if (auth == NULL) {
        dmd_log(LOG_DEBUG, "client authentication is NULL\n");
    } else {
        dmd_log(LOG_DEBUG, "client authentication is not NULL\n");
    }
    ret = response_url(client_fd, url, auth);

    if (auth != NULL) {
        free(auth);
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

        close(client_fd); //remember to close client fd!
    }

#undef BUFFSIZE

    return 0;
}

