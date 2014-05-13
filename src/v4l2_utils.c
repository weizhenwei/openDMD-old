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
 * File: v4l2_utils.c
 *
 * Brief: Wrapper functions about v4l2 api originate from <linux/videodev2.h>
 *
 * Date: 2014.05.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "v4l2_utils.h"
#include "dmd_log.h"

/*
 * DRIVER CAPABILITIES
 *
 * struct v4l2_capability {
 * 	__u8	driver[16];	// i.e. "bttv"
 * 	__u8	card[32];	// i.e. "Hauppauge WinTV"
 * 	__u8	bus_info[32];	// "PCI:" + pci_name(pci_dev)
 * 	__u32   version;        // should use KERNEL_VERSION()
 * 	__u32	capabilities;	// Device capabilities,
 * 				// for more details, see linux/videodev2.h
 * 	__u32	reserved[4];
 * };
 *
 * query video device's capability
 */
int video_capability(struct v4l2_device_info *v4l2_info)
{
    int ret = 0;

    // get the device capability.
    struct v4l2_capability *capture = &v4l2_info->cap;
    if ((ret = ioctl(v4l2_info->video_device_fd, VIDIOC_QUERYCAP, capture)) == -1) {
	dmd_log(LOG_ERR, "query device capability failed.\n");
	return ret;
    }

    dmd_log(LOG_INFO, "**********Device Capability informations**********\n");
    dmd_log(LOG_INFO, "driver: %s\n", capture->driver);
    dmd_log(LOG_INFO, "card: %s\n", capture->card);
    dmd_log(LOG_INFO, "bus_info: %s\n", capture->bus_info);
    dmd_log(LOG_INFO, "version: %x\n", capture->version);
    dmd_log(LOG_INFO, "capabilities: 0X%x\n", capture->capabilities);
    if (capture->capabilities & V4L2_CAP_VIDEO_CAPTURE) {
	dmd_log(LOG_INFO, "Capture capability is supported\n");
    } else {
	dmd_log(LOG_INFO, "Capture capability is not supported\n");

    }
    if (capture->capabilities & V4L2_CAP_VIDEO_OUTPUT) {
	dmd_log(LOG_INFO, "Output capability is supported\n");
    } else {
	dmd_log(LOG_INFO, "Output capability is not supported\n");

    }
    if (capture->capabilities & V4L2_CAP_STREAMING) {
	dmd_log(LOG_INFO, "Streaming capability is supported\n");
    } else {
	dmd_log(LOG_INFO, "Streaming capability is not supported\n");
    }

    return ret;
}

