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

#ifndef HTTP_RESPONSE_INL_H
#define HTTP_RESPONSE_INL_H

static const char *ok_response_header =
    "HTTP/1.1 200 ok\r\n"
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

static const char * not_found_response =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<head>\n"
    "<title>Not Found</title>\n"
    "</head>"
    "<body>\n"
    "<h1>Not Found</h1>\n"
    "<p>The requested URL was not found on the server.</p>\n"
    "</body>\n"
    "</html>\n";

static const char *bad_method_response =
    "HTTP/1.0 501 Method Not Implemented\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<body>\n"
    "<h1>Method Not Implemented</h1>\n"
    "<p>The method is not implemented by this server.</p>\n"
    "</body>\n"
    "</html>\n";

#if 0
static const char *not_valid_response =
    "HTTP/1.0 404 Not Valid\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<body>\n"
    "<h1>Not Valid</h1>\n"
    "<p>The requested URL is not valid.</p>\n"
    "</body>\n"
    "</html>\n";

static const char *not_valid_syntax =
    "HTTP/1.0 404 Not Valid Syntax\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\n"
    "<body>\n"
    "<h1>Not Valid Syntax</h1>\n"
    "</body>\n"
    "</html>\n";

static const char *request_auth_response=
    "HTTP/1.0 401 Authorization Required\r\n"
    "WWW-Authenticate: Basic realm=\"openDMD Security Access\"\r\n";
#endif

#endif
