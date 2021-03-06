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

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <locale.h>
#include <linux/limits.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/client/image_capture.h"
#include "src/client/image_convert.h"
#include "src/client/picture_thread.h"
#include "src/client/video.h"
#include "src/client/video_thread.h"
#include "src/client/rtp_send.h"
#include "src/server/rtp_server.h"
#include "src/webserver/webserver_process.h"
#include "src/config.h"
#include "src/global_context.h"
#include "src/log.h"
#include "src/parser.h"
#include "src/path.h"
#include "src/signal_handler.h"
#include "src/sqlite_utils.h"
#include "src/statistics.h"
#include "src/v4l2_utils.h"

// for cmd line control
static unsigned int show_statistics = 0;
static unsigned int clean_statistics = 0;
static unsigned int start_webadmin = 0;

static void clean(void) {
    dmd_closelog();
}

static void client_working_progress() {
    int ret = -1;
    const char *devpath = global.client.video_device;

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

// daemonize the main program;
static void daemonize() {
    pid_t pid;
    int i, fd0, fd1, fd2;
    struct rlimit r1;
    struct sigaction sa;
    FILE *fp = NULL;

    // clear file creation mask.
    umask(0);

    // get maximum numbers of file descriptors.
    if (getrlimit(RLIMIT_NOFILE, &r1) == -1) {
        dmd_log(LOG_ERR, "getrlimit-file limit error:%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // fork and parent exits.
    if ((pid = fork()) == -1) {
        dmd_log(LOG_ERR, "fork error:%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // parent
        exit(EXIT_SUCCESS);
    }

    // the folloing is runing by child.

    // become a session leader to lose controlling  TTY.
    if ((pid = setsid()) == -1) {
        dmd_log(LOG_ERR, "setsid error:%s\n", strerror(errno));
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
        dmd_log(LOG_ERR, "fork 2 error:%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // parent
        exit(EXIT_SUCCESS);
    }

    // change the current working directory to the root
    // so we won't prevent file systems from being unmounted.
    if (chdir("/") == -1) {
        dmd_log(LOG_ERR, "chdir to / error:%s\n", strerror(errno));
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
    global.pid = pid;  // refresh global's pid member;
    if ((fp = fopen(global.pid_file, "w")) == NULL) {
        dmd_log(LOG_ERR, "fopen pid file error:%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%d", pid);
    fclose(fp);
}

static void init(void) {
    int ret = -1;
    // signal init;
    signal_init();

    // parse config file;
    assert(global.cfg_file != NULL);
    ret = parse_config(global.cfg_file);
    assert(ret == 0);

#if defined(DEBUG)
    ret = dump_global_config();
    assert(ret == 0);
#endif

    // init statistics global variable;
    global_stats = new_statistics();

    // database file initialize
    init_database();

    // daemonize;
    if (global.daemon_mode == DAEMON_ON) {
        daemonize();
    }
}

static void client_rtp_init() {
    global.client.clientrtp.server_rtp_port =
        global.client.clientrtp.server_port_base +
        2 * global.client.clientrtp.local_sequence_number;

    global.client.clientrtp.server_rtcp_port =
        global.client.clientrtp.server_port_base +
        2 * global.client.clientrtp.local_sequence_number + 1;

    rtp_send_init();

    global.client.clientrtp.rtpsession = rtp_send_createSession(
            global.client.clientrtp.local_ip,
            global.client.clientrtp.local_port,
            global.client.clientrtp.server_ip,
            global.client.clientrtp.server_rtp_port);
}

static void client_rtp_release() {
    rtp_send_release(global.client.clientrtp.rtpsession);
}

static void client_create_thread() {
    int dummy = 0;
    int ret;

    if (global.client.working_mode == CAPTURE_ALL) {
        // create picture thread;
        ret = pthread_create(&global.client.thread_attr.picture_thread_id,
                &global.client.thread_attr.global_attr,
                picture_thread, &dummy);
        assert(ret == 0);

        pthread_mutex_lock(&total_thread_mutex);
        total_thread++;
        pthread_mutex_unlock(&total_thread_mutex);

        // and create video thread;
        ret = pthread_create(&global.client.thread_attr.video_thread_id,
                &global.client.thread_attr.global_attr, video_thread, &dummy);
        assert(ret == 0);

        pthread_mutex_lock(&total_thread_mutex);
        total_thread++;
        pthread_mutex_unlock(&total_thread_mutex);
    } else if (global.client.working_mode == CAPTURE_PICTURE) {
        // only create picture thread;
        ret = pthread_create(&global.client.thread_attr.picture_thread_id,
                &global.client.thread_attr.global_attr, picture_thread, &dummy);
        assert(ret == 0);

        pthread_mutex_lock(&total_thread_mutex);
        total_thread++;
        pthread_mutex_unlock(&total_thread_mutex);
    } else if (global.client.working_mode == CAPTURE_VIDEO) {
        // only create video thread;
        ret = pthread_create(&global.client.thread_attr.video_thread_id,
                &global.client.thread_attr.global_attr, video_thread, &dummy);
        assert(ret == 0);

        pthread_mutex_lock(&total_thread_mutex);
        total_thread++;
        pthread_mutex_unlock(&total_thread_mutex);

    } else {
        dmd_log(LOG_ERR, "impossible reach here!\n");
        assert(0);
    }
}

static void usage(const char *progname) {
    fprintf(stdout, "Usage:%s [OPTION...]\n", progname);
    fprintf(stdout, "  -p, --pid-file=FILE,    Use specified pid file\n");
    fprintf(stdout, "  -f, --cfg-file=FILE,    Use specified config file\n");
    fprintf(stdout, "  -d, --daemonize,        Run opendmd in daemon mode\n");
    fprintf(stdout, "  -v, --version,          Display the version number\n");
    fprintf(stdout, "  -s, --show-statistics,  Show historical statistics\n");
    fprintf(stdout, "  -w, --start-webadmin,   Start web administration UI\n");
    fprintf(stdout, "  -c, --clean-statistics, Clean historical statistics\n");
    fprintf(stdout, "  -h, --help,             Display this help message\n");
}

static int parse_cmdline(int argc, char *argv[]) {
    int c;
    struct option long_options[] = {
        {"pid-file",           required_argument,  0, 'p'},
        {"cfg-file",           required_argument,  0, 'f'},
        {"daemonize",          no_argument,        0, 'd'},
        {"version",            no_argument,        0, 'v'},
        {"show-statistics",    no_argument,        0, 's'},
        {"start-webadmin",     no_argument,        0, 'w'},
        {"clean-statistics",   no_argument,        0, 'c'},
        {"help",               no_argument,        0, 'h'},
        {0, 0, 0, 0},
    };

    // man 3 getopt_long for more information about getopt_long;
    while ((c = getopt_long(argc, argv, "p:f:dvswch", long_options, NULL))
            != EOF) {
        switch (c) {
            case 'v':
                fprintf(stdout, "%s\n", "opendmd 0.0.1");
                exit(EXIT_SUCCESS);
                break;
            case 's':
                show_statistics = 1;
                break;
            case '2':
                start_webadmin = 1;
                break;
            case 'c':
                clean_statistics = 1;
                break;
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'd':
                global.daemon_mode = DAEMON_ON;
                break;
            case 'p':
                assert(strlen(optarg) < PATH_MAX);
                strncpy(global.pid_file, optarg, strlen(optarg));
                /* Warning:If there is no null byte among the first n bytes of
                 * src, the string placed in dest will not be null-terminated,
                 * remember add null-terminated manually.
                 */
                global.pid_file[strlen(optarg)] = '\0';
                break;
            case 'f':
                assert(strlen(optarg) < PATH_MAX);
                strncpy(global.cfg_file, optarg, strlen(optarg));
                /* Warning:If there is no null byte among the first n bytes of
                 * src, the string placed in dest will not be null-terminated,
                 * remember add null-terminated manually.
                 */
                global.cfg_file[strlen(optarg)] = '\0';
                break;
            default:
                exit(EXIT_SUCCESS);
                break;
        }
    }  // while

    if (optind < argc) {
        fprintf(stderr, "Illegal argument(s): ");
        while (optind < argc) {
            fprintf(stderr, "%s ", argv[optind++]);
            fprintf(stderr, "\n");
        }
        exit(EXIT_FAILURE);
    }

    return 0;
}

static void manage_cmdline() {
    if (show_statistics == 1 && clean_statistics == 1) {
        fprintf(stderr, "ERROR: show statistics and clean statistics "
                "can't be specified at same time.\n");
        exit(EXIT_FAILURE);
    }

    if (show_statistics == 1) {
        init_database();
        dump_database_table(opendmd_db, DEFAULT_TABLE);
        close_db(opendmd_db);

        exit(EXIT_SUCCESS);
    }

    if (clean_statistics == 1) {
        init_database();
        clean_database_table(opendmd_db, DEFAULT_TABLE);
        close_db(opendmd_db);

        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    int ret = -1;

    // set locale according current environment;
    setlocale(LC_ALL, "");

    // open log first;
    dmd_openlog(DMD_IDENT, DMD_LOGOPT, DMD_FACILITY);

    // initialize struct global_context global to default value;
    init_default_global();

    // parse command line parameters
    parse_cmdline(argc, argv);
    // manage cmd line options;
    manage_cmdline();

    init();

    // fork webui process;
    if (start_webadmin == 1) {
        ret = webserver_fork();
        assert(ret == 0);
    }

    // slave or singleton do the capturing work;
    if (global.cluster_mode == CLUSTER_CLIENT
            || global.cluster_mode == CLUSTER_SINGLETON) {
        client_init_repodir();
        if (global.cluster_mode == CLUSTER_CLIENT) {
            client_rtp_init();
        }

        // create picture thread and/or video thread;
        client_create_thread();
        client_working_progress();

        if (global.cluster_mode == CLUSTER_CLIENT) {
            client_rtp_release();
        }
    } else if (global.cluster_mode == CLUSTER_SERVER) {
        if (rtp_server_init() != 0) {
            dmd_log(LOG_ERR, "rtp_server_init failed, the program exit!\n");
            goto end;
        }

        rtp_server_running();
        rtp_server_clean();
    }

end:
    // kill webserver process;
    if (start_webadmin == 1) {
        ret = kill(global.webserver_pid, SIGKILL);
        assert(ret == 0);
    }

    release_default_global();
    clean();

    return 0;
}

