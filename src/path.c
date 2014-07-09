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

#include "path.h"

static int test_and_create(const char *path)
{
    if (access(path, F_OK) == -1) { // if path didn't exist, mkdir it;
        if (mkdir(path, 0755) == -1) {
            dmd_log(LOG_ERR, "mkdir %s error:%s\n", path, strerror(errno));
            return -1;
        }
    } else { // path exists, be sure it is directory;
        struct stat path_stat;
        if (stat(path, &path_stat) == -1) {
            dmd_log(LOG_ERR, "stat error:%s\n", path, strerror(errno));
            return -1;
        }
        if (!S_ISDIR(path_stat.st_mode)) { // first delete, then mkdir;
            if (unlink(path) == -1) {
                dmd_log(LOG_ERR, "unlink error:%s", strerror(errno));
                return -1;
            }
            if (mkdir(path, 0755) == -1) {
                dmd_log(LOG_ERR, "mkdir %s error:%s\n", path, strerror(errno));
                return -1;
            }
        }
    } // else

    return 0;
}

char *get_jpeg_filepath()
{
    time_t now;
    struct tm *tmptr;
    // at linux/limits.h, #define PATH_MAX 4096
    static char filepath[PATH_MAX];
    char storepath[PATH_MAX];
    strncpy(storepath, global.client.store_dir,
            strlen(global.client.store_dir));
    storepath[strlen(global.client.store_dir)] = '\0';
    strcat(storepath, "/jpeg");
    assert(test_and_create(storepath) == 0);

    now = time(&now);
    assert(now != -1);

    tmptr = localtime(&now);
    assert(tmptr != NULL);
    sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d-%02d.jpg",
            storepath, 
            tmptr->tm_year + 1900,
            tmptr->tm_mon + 1,
            tmptr->tm_mday,
            tmptr->tm_hour,
            tmptr->tm_min,
            tmptr->tm_sec,
            global.client.counter_in_second);

    assert(strlen(filepath) < PATH_MAX);

    // dmd_log(LOG_INFO, "in function %s, jpeg filename is: %s\n",
    //         __func__, filepath);

    return filepath;
}

char *get_h264_filepath()
{
    time_t now;
    struct tm *tmptr;
    // at linux/limits.h, #define PATH_MAX 4096
    static char filepath[PATH_MAX];
    char storepath[PATH_MAX];
    strncpy(storepath, global.client.store_dir,
            strlen(global.client.store_dir));
    storepath[strlen(global.client.store_dir)] = '\0';
    strcat(storepath, "/h264");
    assert(test_and_create(storepath) == 0);

    now = time(&now);
    assert(now != -1);

    tmptr = localtime(&now);
    assert(tmptr != NULL);
    // sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d.h264",
    sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d.h264",
            storepath,
            tmptr->tm_year + 1900,
            tmptr->tm_mon + 1,
            tmptr->tm_mday,
            tmptr->tm_hour,
            tmptr->tm_min,
            tmptr->tm_sec);
    assert(strlen(filepath) < PATH_MAX);

    dmd_log(LOG_DEBUG, "in function %s, h264 filename is: %s\n",
           __func__, filepath);

    return filepath;
}

char *get_flv_filepath()
{
    time_t now;
    struct tm *tmptr;
    // at linux/limits.h, #define PATH_MAX 4096
    static char filepath[PATH_MAX];
    char storepath[PATH_MAX];
    strncpy(storepath, global.client.store_dir,
            strlen(global.client.store_dir));
    storepath[strlen(global.client.store_dir)] = '\0';
    strcat(storepath, "/h264");
    assert(test_and_create(storepath) == 0);

    now = time(&now);
    assert(now != -1);

    tmptr = localtime(&now);
    assert(tmptr != NULL);
    // sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d.h264",
    sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d.flv",
            storepath,
            tmptr->tm_year + 1900,
            tmptr->tm_mon + 1,
            tmptr->tm_mday,
            tmptr->tm_hour,
            tmptr->tm_min,
            tmptr->tm_sec);
    assert(strlen(filepath) < PATH_MAX);

    dmd_log(LOG_DEBUG, "in function %s, h264 filename is: %s\n",
           __func__, filepath);

    return filepath;
}

int server_init_repodir()
{
    return test_and_create(global.server.server_repo);
}

int server_init_client_repodir(int client_number)
{
    char client_repodir[PATH_MAX];

    sprintf(client_repodir, "%s/client-%02d",
            global.server.server_repo, client_number);

    dmd_log(LOG_DEBUG, "in function %s,  client repodir is : %s\n",
           __func__, client_repodir);
    
    return test_and_create(client_repodir);
}

char *server_get_h264_filepath(int client_number)
{
    time_t now;
    struct tm *tmptr;
    // at linux/limits.h, #define PATH_MAX 4096
    static char filepath[PATH_MAX];
    char storepath[PATH_MAX];
    sprintf(storepath, "%s/client-%02d",
            global.server.server_repo, client_number);
    assert(test_and_create(storepath) == 0);

    now = time(&now);
    assert(now != -1);

    tmptr = localtime(&now);
    assert(tmptr != NULL);
    sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d.h264",
            storepath,
            tmptr->tm_year + 1900,
            tmptr->tm_mon + 1,
            tmptr->tm_mday,
            tmptr->tm_hour,
            tmptr->tm_min,
            tmptr->tm_sec);
    assert(strlen(filepath) < PATH_MAX);

    dmd_log(LOG_DEBUG, "in function %s, h264 filename is: %s\n",
           __func__, filepath);

    return filepath;
}

char *server_get_flv_filepath(int client_number)
{
    time_t now;
    struct tm *tmptr;
    // at linux/limits.h, #define PATH_MAX 4096
    static char filepath[PATH_MAX];
    char storepath[PATH_MAX];
    sprintf(storepath, "%s/client-%02d",
            global.server.server_repo, client_number);
    assert(test_and_create(storepath) == 0);

    now = time(&now);
    assert(now != -1);

    tmptr = localtime(&now);
    assert(tmptr != NULL);
    sprintf(filepath, "%s/%04d%02d%02d%02d%02d%02d.flv",
            storepath,
            tmptr->tm_year + 1900,
            tmptr->tm_mon + 1,
            tmptr->tm_mday,
            tmptr->tm_hour,
            tmptr->tm_min,
            tmptr->tm_sec);
    assert(strlen(filepath) < PATH_MAX);

    dmd_log(LOG_DEBUG, "in function %s, h264 filename is: %s\n",
           __func__, filepath);

    return filepath;
}
