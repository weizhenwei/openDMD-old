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
 * File: log.c
 *
 * Brief: log utility of the project
 *
 * Date: 2014.05.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "log.h"

void dmd_openlog(const char *ident, int logopt, int facility)
{
    openlog(ident, logopt, facility);
}

void dmd_log(int priority, const char *format, ...)
{

    if (priority > global.log_level) {
        return;
    }
    if (format == NULL) {
        return;
    }

    va_list var_list;

    va_start(var_list, format);
    va_end(var_list);
    vsyslog(priority, format, var_list);

#if defined(DEBUG)
    // print to stdout;

    char *log_level = NULL;

    if (priority == LOG_INFO) {
        log_level = "info";
    } else if (priority == LOG_ERR) {
        log_level = "error";
    } else if (priority == LOG_DEBUG) {
        log_level = "debug";
    } else if (priority == LOG_EMERG) {
        log_level = "emergent";
    } else if (priority == LOG_ALERT) {
        log_level = "alert";
    } else if (priority == LOG_CRIT) {
        log_level = "critical";
    } else if (priority == LOG_WARNING) {
        log_level = "warning";
    } else if (priority == LOG_NOTICE) {
        log_level = "notice";
    }

    // TODO: why va_start and va_end again ?
    va_start(var_list, format);
    va_end(var_list);
    fprintf(stdout, "[%s]: ", log_level);
    vfprintf(stdout, format, var_list);
#endif
}

void dmd_closelog()
{
    closelog();
}
