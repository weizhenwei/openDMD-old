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
 * File: main.c
 *
 * Brief: main entry point of the project
 *
 * Date: 2014.05.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "dmd_log.h"
#include "dmd_video.h"
#include "dmd_signal.h"
#include "dmd_v4l2_utils.h"
#include "dmd_image_capture.h"
#include "dmd_global_context.h"

#define PIDFILE "/home/wzw/openDMD/opendmd.pid"

extern struct v4l2_device_info *dmd_video;

extern time_t lasttime;
extern unsigned short int counter_in_minute;

extern char *h264_filename;
extern struct global_context global;
extern struct global_context default_context;


void clean(void)
{
    dmd_closelog();
}

void working_progress()
{
    int ret = -1;
    const char *devpath = DEVICE_PATH;

    dmd_video = dmd_video_create(devpath);
    assert(dmd_video != NULL);

    ret = dmd_video_open(dmd_video);
    assert(ret != -1);

    ret = dmd_video_init(dmd_video);
    assert(ret != -1);

    ret = dmd_video_streamon(dmd_video);
    assert(ret != -1);

    ret = dmd_image_capture(dmd_video);
    assert(ret != -1);

    ret = dmd_video_streamoff(dmd_video);
    assert(ret != -1);

    ret = dmd_video_close(dmd_video);
    assert(ret != -1);

    dmd_video_release(dmd_video);
}

void daemonize()
{
    pid_t pid;
    int i, fd0, fd1, fd2;
    struct rlimit r1;
    struct sigaction sa;
    FILE *fp = NULL;

    // clear file creation mask.
    umask(0);

    // get maximum numbers of file descriptors.
    if (getrlimit(RLIMIT_NOFILE, &r1) == -1) {
        dmd_log(LOG_ERR, "getrlimit-file limit error.\n");
        exit(EXIT_FAILURE);
    }

    // fork and parent exits.
    if ((pid = fork()) == -1) {
        dmd_log(LOG_ERR, "fork error.\n");
        exit(EXIT_FAILURE);
    } else if (pid > 0) { // parent
        exit(EXIT_SUCCESS);
    }

    // the folloing is runing by child.

    // become a session leader to lose controlling  TTY.
    if ((pid = setsid()) == -1) {
        dmd_log(LOG_ERR, "setsid error.\n");
        exit(EXIT_FAILURE);
    }

    // set SIGHUP sighandler;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        dmd_log(LOG_ERR, "sigaction error.\n");
        exit(EXIT_FAILURE);
    }

    // fork again, the grandchild alive since then.
    if ((pid = fork()) == -1) {
        dmd_log(LOG_ERR, "fork 2 error.\n");
        exit(EXIT_FAILURE);
    } else if (pid > 0) { // parent
        exit(EXIT_SUCCESS);
    }

    // change the current working directory to the root
    // so we won't prevent file systems from being unmounted.
    if (chdir("/") == -1) {
        dmd_log(LOG_ERR, "chdir to root error.\n");
        exit(EXIT_FAILURE);
    }

    // close all open file descriptors.
    if (r1.rlim_max == RLIM_INFINITY) {
        r1.rlim_max = 1024;
    }
    for (i = 0; i < r1.rlim_max; i++) {
        close(i);
    }

    // attach file descriptors 0, 1, and 2 to /dev/null.
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        dmd_log(LOG_ERR, "unexpected file descriptors: %d, %d, %d\n",
                fd0, fd1, fd2);
        exit(EXIT_FAILURE);
    }

    // write pid to file
    pid = getpid();
    if ((fp = fopen(PIDFILE, "w")) == NULL) {
        dmd_log(LOG_ERR, "fopen PIDFILE error.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%d", pid);
    fclose(fp);

}

void init(void)
{
    // signal init;
    signal_init();

    lasttime = time(&lasttime);
    counter_in_minute = 0;

    h264_filename = get_h264_filepath();
    assert(h264_filename != NULL);

    dmd_openlog(DMD_IDENT, DMD_LOGOPT, DMD_FACILITY);

    global = default_context;

    // daemonize();
}

int main(int argc, char *argv[])
{
    // set locale according current environment
    setlocale(LC_ALL, "");

    init();

    working_progress();

    clean();

    return 0;
}
