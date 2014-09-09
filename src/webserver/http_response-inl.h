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
 * File: http_response-inl.h
 *
 * Brief: various http response header definition;
 *
 * Date: 2014.09.01
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef SRC_WEBSERVER_HTTP_RESPONSE_INL_H_
#define SRC_WEBSERVER_HTTP_RESPONSE_INL_H_

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "src/log.h"


static const char *ok_response_header =
    "HTTP/1.0 200 ok\r\n"
    "Server: openDMD-0.01\r\n"
    "Connection: close\r\n"
    "Max-Age: 0\r\n"
    "Expires: 0\r\n"
    "Cache-Control: no-cache\r\n"
    "Cache-Control: private\r\n"
    "Pragma: no-cache\r\n"
    "Content-type: text/html\r\n\r\n";

static const char *bad_request_response =
    "HTTP/1.0 400 Bad Request\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<body>\n"
    "<h1>Bad Request</h1>\n"
    "<p>The server did not understand your request.</p>\n"
    "</body>\n"
    "</html>\n";

static const char * forbidden_response =
    "HTTP/1.0 403 Forbidden\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<head>\n"
    "<title>Forbidden</title>\n"
    "</head>"
    "<body>\n"
    "<h1>Forbidden</h1>\n"
    "<p>The requested URL was forbidden to access.</p>\n"
    "</body>\n"
    "</html>\n";

static const char * not_found_response =
    "HTTP/1.0 404 Not Found\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<head>\n"
    "<title>Not Found</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Not Found</h1>\n"
    "<p>The requested URL was not found on the server.</p>\n"
    "</body>\n"
    "</html>\n";

static const char *method_not_implemented_response =
    "HTTP/1.0 501 Method Not Implemented\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<head>\n"
    "<title>Method Not Implemented</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Method Not Implemented</h1>\n"
    "<p>The method is not implemented by this server.</p>\n"
    "</body>\n"
    "</html>\n";

static const char *not_valid_response =
    "HTTP/1.0 404 Not Valid\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<head>\n"
    "<title>Not Valid</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Not Valid</h1>\n"
    "<p>The requested URL is not valid.</p>\n"
    "</body>\n"
    "</html>\n";

static const char *auth_response =
    "HTTP/1.0 401 Authorization Required\r\n"
    "WWW-Authenticate: Basic realm=\"openDMD Security Access\"\r\n";

static const char *response_header =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; "
    "charset=UTF-8\" />\n"
    "<title>%s</title>\n"
    "</head>\n";

static const char *response_css =
    "<style type=\"text/css\" media=\"screen\">\n"
    "body { background: #e7e7e7; font-family: Verdana, sans-serif;"
    "font-size: 11pt; }\n"
    "#page { background: #ffffff; margin: 50px; border: 2px solid #c0c0c0;"
    "padding: 10px; }\n"
    "#header { background: #4b6983; border: 2px solid #7590ae;"
    "text-align: center; padding: 10px; color: #ffffff; }\n"
    "#header h1 { color: #ffffff; }\n"
    "#body { padding: 10px; }\n"
    "span.tt { font-family: monospace; }\n"
    "span.bold { font-weight: bold; }\n"
    "a:link { text-decoration: none; font-weight: bold; color: #0000FF; }\n"
    "a:visited { text-decoration: none; font-weight: bold; color: #800080; }\n"
    "a:active { text-decoration: none; font-weight: bold; color: #4B0082;}\n"
    "a:hover { text-decoration: none; color: #2F4F4F;}\n"
    "</style>\n";

static const char *response_footer = "</html>";

static inline void send_not_found_response(int client_fd) {
    int not_found_len = strlen(not_found_response);
    int sendlen = send(client_fd, not_found_response, not_found_len, 0);
    assert(sendlen == not_found_len);
    dmd_log(LOG_DEBUG, "send not_found_response to client:\n%s\n",
            not_found_response);
}

static inline void send_bad_request_response(int client_fd) {
    int bad_request_len = strlen(bad_request_response);
    int sendlen = send(client_fd, bad_request_response, bad_request_len, 0);
    assert(sendlen == bad_request_len);
    dmd_log(LOG_DEBUG, "send bad_request_response to client:\n%s\n",
            bad_request_response);
}

static inline void send_ok_response_header(int client_fd) {
    int ok_len = strlen(ok_response_header);
    int sendlen = send(client_fd, ok_response_header, ok_len, 0);
    assert(sendlen == ok_len);
    dmd_log(LOG_DEBUG, "send ok_response_header to client:\n%s\n",
            ok_response_header);
}

static inline void send_forbidden_response(int client_fd) {
    int forbidden_len = strlen(forbidden_response);
    int sendlen = send(client_fd, forbidden_response, forbidden_len, 0);
    assert(sendlen == forbidden_len);
    dmd_log(LOG_DEBUG, "send forbidden_response to client:\n%s\n",
            forbidden_response);
}

static inline void send_not_valid_response(int client_fd) {
    int not_valid_len = strlen(not_valid_response);
    int sendlen = send(client_fd, not_valid_response, not_valid_len, 0);
    assert(sendlen == not_valid_len);
    dmd_log(LOG_DEBUG, "send not_valid_response to client:\n%s\n",
            not_valid_response);
}

static inline void send_method_not_implemented_response(int client_fd) {
    int not_implemented_len = strlen(method_not_implemented_response);
    int sendlen = send(client_fd, method_not_implemented_response,
            not_implemented_len, 0);
    assert(sendlen == not_implemented_len);
    dmd_log(LOG_DEBUG, "send method_not_implemented_response to client:\n%s\n",
            method_not_implemented_response);
}

static void send_authentication(int client_fd) {
    int auth_response_len = strlen(auth_response);
    int sendlen = send(client_fd, auth_response, auth_response_len, 0);
    assert(sendlen == auth_response_len);
    dmd_log(LOG_DEBUG, "send auth_response to client:\n%s\n", auth_response);
}

static inline void send_response_header(int client_fd, const char *title) {
    char header[1024];
    sprintf(header, response_header, title);
    int header_len = strlen(header);
    int sendlen = send(client_fd, header, header_len, 0);
    assert(sendlen == header_len);
    dmd_log(LOG_DEBUG, "send response header to client:\n%s\n",
            header);
}

static inline void send_css(int client_fd) {
    int css_len = strlen(response_css);
    int sendlen = send(client_fd, response_css, css_len, 0);
    assert(sendlen == css_len);
    dmd_log(LOG_DEBUG, "send response css to client:\n%s\n",
            response_css);
}

static inline void send_response_footer(int client_fd) {
    int footer_len = strlen(response_footer);
    int sendlen = send(client_fd, response_footer, footer_len, 0);
    assert(sendlen == footer_len);
    dmd_log(LOG_DEBUG, "send response footer to client:\n%s\n",
            response_footer);
}


#endif  // SRC_WEBSERVER_HTTP_RESPONSE_INL_H_
