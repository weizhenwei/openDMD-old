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
 * File: dmd_image.c
 *
 * Brief: process the captured image from video device. 
 *
 * Date: 2014.05.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "dmd_image.h"

static int process_image(void *addr, int length)
{
    FILE *fp;
    static int num = 0;
    // at linux/limits.h, #define PATH_MAX 4096
    char image_name[PATH_MAX];

    sprintf(image_name, FILE_NAME, num++);
    fp = fopen(image_name, "wb");
    if (fp == NULL) {
	dmd_log(LOG_ERR, "fopen error.\n");
	return -1;
    }

    fwrite(addr, length, 1, fp);
    usleep(500);

    return 0;
}

static int read_frame(int fd, struct mmap_buffer *buffers)
{
    int ret = 0;
    struct v4l2_buffer buf;
    unsigned int i;
    bzero(&buf, sizeof(struct v4l2_buffer));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if ((ret = ioctl(fd, VIDIOC_DQBUF, &buf)) == -1) {
	dmd_log(LOG_ERR, "ioctl VIDIOC_DQBUF failed.\n");
	return ret;
    }

    // read process space's data to a file
    process_image(buffers[buf.index].start, buffers[buf.index].length);

    // put buf back to queue
    if ((ret = ioctl(fd, VIDIOC_QBUF, &buf)) == -1) {
	dmd_log(LOG_ERR, "ioctl VIDIOC_QBUF failed.\n");
	return ret;
    }

    return ret;
}


int dmd_image_capture(struct v4l2_device_info *v4l2_info)
{
    unsigned int count = 5;

    int fd = v4l2_info->video_device_fd;
    struct mmap_buffer *buffers = v4l2_info->buffers;
    
    while (count-- > 0) {
	for (;;) {
	    fd_set fds;
	    struct timeval tv;
	    int r;

	    FD_ZERO(&fds);
	    FD_SET(fd, &fds);

	    // timeout
	    tv.tv_sec = 2;
	    tv.tv_usec = 0;

	    r = select(fd + 1, &fds, NULL, NULL, &tv);
	    if ( r == -1) {
		if (errno == EINTR)
		    continue;
		dmd_log(LOG_ERR, "Multi I/O select failed.\n");
		return -1;
	    } else if (r == 0) {
		dmd_log(LOG_ERR, "Multi I/O select timeout.\n");
		return -1;
	    }

	    if (read_frame(fd, buffers) == 0)
		break;
	}
    }

    return 0;
}
