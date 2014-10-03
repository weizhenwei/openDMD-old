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
 * File: path.c
 *
 * Brief: file path operation for storing picture and video. 
 *
 * Date: 2014.06.07
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "src/path.h"

#define _GNU_SOURCE  // for strndupa() function;

#include <assert.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "src/global_context.h"
#include "src/log.h"

int test_and_mkdir(const char *path) {
    // first, find parent path;
    const char *ptr = path + strlen(path);
    while (*ptr != '/' && ptr > path) {
        ptr--;
    }
    char *parent = strndupa(path, ptr - path);  // no need to free manually
    assert(parent != NULL);
    dmd_log(LOG_DEBUG, "in function %s, parent path is %s\n",
            __func__, parent);
    dmd_log(LOG_DEBUG, "in function %s, path is %s\n",
            __func__, path);

    if (access(parent, F_OK) == 0) {  // parent exist, just mkdir
        if (access(path, F_OK) == 0) {  // path exist, just return
            dmd_log(LOG_DEBUG, "in function %s, dir %s existed already\n",
                    __func__, path);
            return 0;
        } else {  // else just mkdir path;
            if (mkdir(path, 0755) == -1) {
                dmd_log(LOG_ERR, "in function %s, mkdir %s error:%s\n",
                        __func__, path, strerror(errno));
                return -1;
            } else {
                dmd_log(LOG_DEBUG, "in function %s, mkdir %s succeed\n",
                        __func__, path);
            }
        }

    } else {  // parent doesn't exist, recursively call test_and_mkdir();
        // first mkdir parent;
        int ret = test_and_mkdir(parent);
        if (ret == -1) {
            return ret;
        }

        // then mkdir path;
        if (mkdir(path, 0755) == -1) {
            dmd_log(LOG_ERR, "in function %s, mkdir %s error:%s\n",
                    __func__, path, strerror(errno));
            return -1;
        } else {
            dmd_log(LOG_DEBUG, "in function %s, mkdir2 %s succeed\n",
                    __func__, path);
        }
    }

    return 0;
}

// client path stuff;
int client_init_repodir() {
    return test_and_mkdir(global.client.client_repo);
}

struct path_t *client_get_filepath(int path_type) {
    char filepath[PATH_MAX];  // #define PATH_MAX 4096 at linux/limits.h
    char storepath[PATH_MAX];

    char *suffix = NULL;
    if (path_type == JPEG_FILE) {
        suffix = "jpg";
    } else if (path_type == H264_FILE) {
        suffix = "h264";
    } else if (path_type == FLV_FILE) {
        suffix = "flv";
    } else {
        dmd_log(LOG_ERR, "in function %s, error to reach here!\n", __func__);
        return NULL;
    }
    // global.client.client_repo's correctness is checked at
    // check_path() at src/config.c
    snprintf(storepath, PATH_MAX, "%s/%s", global.client.client_repo, suffix);
    assert(test_and_mkdir(storepath) == 0);

    time_t now;
    struct tm tmp = {0};
    struct tm *tmptr = &tmp;
    now = time(&now);
    assert(now != -1);
    tmptr = localtime_r(&now, tmptr);
    assert(tmptr != NULL);

    if (strcmp(suffix, "jpg") == 0) {
        snprintf(filepath, PATH_MAX, "%s/%04d%02d%02d%02d%02d%02d-%02d.%s",
                storepath,
                tmptr->tm_year + 1900,
                tmptr->tm_mon + 1,
                tmptr->tm_mday,
                tmptr->tm_hour,
                tmptr->tm_min,
                tmptr->tm_sec,
                global.client.counter_in_second,
                suffix);
    } else {
        snprintf(filepath, PATH_MAX, "%s/%04d%02d%02d%02d%02d%02d.%s",
                storepath,
                tmptr->tm_year + 1900,
                tmptr->tm_mon + 1,
                tmptr->tm_mday,
                tmptr->tm_hour,
                tmptr->tm_min,
                tmptr->tm_sec,
                suffix);
    }
    assert(strlen(filepath) < PATH_MAX);

    struct path_t *path = (struct path_t *)malloc(sizeof(struct path_t));
    assert(path != NULL);
    int len = strlen(filepath);
    path->path = (char *)malloc(sizeof(char) * (len + 1));
    assert(path->path != NULL);
    strcpy(path->path, filepath);
    path->len = len;

    dmd_log(LOG_DEBUG, "in function %s, get filename: %s\n",
           __func__, filepath);

    return path;
}

int server_init_repodir() {
    return test_and_mkdir(global.server.server_repo);
}

int server_init_client_repodir(int client_number) {
    char client_repodir[PATH_MAX];

    snprintf(client_repodir, PATH_MAX, "%s/client-%02d",
            global.server.server_repo, client_number);

    dmd_log(LOG_DEBUG, "in function %s,  client repodir is : %s\n",
           __func__, client_repodir);

    return test_and_mkdir(client_repodir);
}


struct path_t *server_get_filepath(int path_type, int client_number) {
    char filepath[PATH_MAX];  // #define PATH_MAX 4096 at linux/limits.h
    char storepath[PATH_MAX];

    char *suffix = NULL;
    if (path_type == JPEG_FILE) {
        suffix = "jpg";
    } else if (path_type == H264_FILE) {
        suffix = "h264";
    } else if (path_type == FLV_FILE) {
        suffix = "flv";
    } else {
        dmd_log(LOG_ERR, "in function %s, error to reach here!\n", __func__);
        return NULL;
    }

    // global.server.server_repo's correctness is checked at
    // check_path() at src/config.c
    snprintf(storepath, PATH_MAX, "%s/client-%02d/%s",
            global.server.server_repo, client_number, suffix);
    assert(test_and_mkdir(storepath) == 0);

    time_t now;
    struct tm tmp = {0};
    struct tm *tmptr = &tmp;
    now = time(&now);
    assert(now != -1);
    tmptr = localtime_r(&now, tmptr);
    assert(tmptr != NULL);

    if (strcmp(suffix, "jpg") == 0) {
        snprintf(filepath, PATH_MAX, "%s/%04d%02d%02d%02d%02d%02d-%02d.%s",
                storepath,
                tmptr->tm_year + 1900,
                tmptr->tm_mon + 1,
                tmptr->tm_mday,
                tmptr->tm_hour,
                tmptr->tm_min,
                tmptr->tm_sec,
                global.client.counter_in_second,
                suffix);
    } else {
        snprintf(filepath, PATH_MAX, "%s/%04d%02d%02d%02d%02d%02d.%s",
                storepath,
                tmptr->tm_year + 1900,
                tmptr->tm_mon + 1,
                tmptr->tm_mday,
                tmptr->tm_hour,
                tmptr->tm_min,
                tmptr->tm_sec,
                suffix);
    }
    assert(strlen(filepath) < PATH_MAX);

    struct path_t *path = (struct path_t *)malloc(sizeof(struct path_t));
    assert(path != NULL);
    int len = strlen(filepath);
    path->path = (char *)malloc(sizeof(char) * (len + 1));
    assert(path->path != NULL);
    strcpy(path->path, filepath);
    path->len = len;

    dmd_log(LOG_DEBUG, "in function %s, get filename: %s\n",
           __func__, filepath);

    return path;
}

