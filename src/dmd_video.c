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
 * File: dmd_video.c
 *
 * Brief: video manipulation interface of the project 
 *
 * Date: 2014.05.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "dmd_video.h"
#include "dmd_log.h"


struct v4l2_device_info *dmd_video_create(const char *device_path)
{
    struct v4l2_device_info *device;
    device = (struct v4l2_device_info *)malloc(sizeof(struct v4l2_device_info));
    if (device == NULL) {
	dmd_log(LOG_ERR, "malloc for struct v4l2_device_info failed.\n");
	return device;
    }

    bzero(device, sizeof(device));

    device->video_device_path = device_path;

    return device;
}

int dmd_video_open(struct v4l2_device_info *v4l2_info)
{
    int fd = -1;
    const char *devpath = v4l2_info->video_device_path;
    dmd_log(LOG_INFO, "video device:%s\n", devpath);
    if ((fd = open(devpath, O_RDWR)) == -1) {
	dmd_log(LOG_ERR, "open video device failded.\n");
	return -1;
    }

    v4l2_info->video_device_fd = fd;

    return 1;
}

int dmd_video_init(struct v4l2_device_info *v4l2_info)
{
    int ret;
    // query video device's capability
    if ((ret = video_capability(v4l2_info)) == -1) {
	return ret;
    }

}

int dmd_video_close(struct v4l2_device_info *v4l2_info)
{
    if (close(v4l2_info->video_device_fd) == -1) {
	dmd_log(LOG_ERR, "close video device failed");
	return -1;
    }

    return 1;
}

void dmd_video_release(struct v4l2_device_info *v4l2_info)
{
    if (v4l2_info != NULL) {
	free(v4l2_info);
    }
}

