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

#include <locale.h>
#include "dmd_log.h"
#include "dmd_video.h"
#include "dmd_signal.h"
#include "dmd_v4l2_utils.h"
#include "dmd_image_capture.h"

extern struct v4l2_device_info *dmd_video;

extern time_t lasttime;
extern unsigned short int counter_in_minute;

void init(void)
{
    // signal init;
    signal_init();

    lasttime = time(&lasttime);
    counter_in_minute = 0;

    dmd_openlog(DMD_IDENT, DMD_LOGOPT, DMD_FACILITY);
}

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

int main(int argc, char *argv[])
{
    // set locale according current environment
    setlocale(LC_ALL, "");

    init();

    working_progress();

    clean();

    return 0;
}
