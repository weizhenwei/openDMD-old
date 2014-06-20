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
 * File: image_convert.h
 *
 * Brief: convert image between different format. 
 *
 * Date: 2014.05.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef IMAGE_CONVERT_H
#define IMAGE_CONVERT_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "image_capture.h"
#include "global_context.h"

// diff with referenceYUYV422 to detect whether motion occured;
extern int YUYV422_motion_detect(unsigned char *yuyv, int width,
        int height, int length);

// rgb format should be the base for futher convert.
extern void YUYV422toRGB888(unsigned char *yuyv, int width,
        int height, unsigned char *rgb, int length);

extern void YUYV422toRGB888INT(unsigned char *yuyv, int width,
        int height, unsigned char *rgb, int length);

// convert packed YUYV422 to planar YUV422P
extern void YUYV422toYUV422P(unsigned char *yuyv422, int width,
        int height, unsigned char *yuv422p, int length);

// convert planar YUV422P to planar YUV420P
extern void YUV422PtoYUV420P(unsigned char *yuv422p, int width,
        int height, unsigned char *yuv420p, int length);

// convert packed YUYV422 to planar YUV420P
extern void YUYV422toYUV420P(unsigned char *yuyv422, int width,
        int height, unsigned char *yuv420p, int length);

#endif