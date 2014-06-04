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
 * File: dmd_image_convert.h
 *
 * Brief: convert image between different format. 
 *
 * Date: 2014.05.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "dmd_image_convert.h"


/*    Packed YUYV data stream: Y0 U0 Y1 V0 Y2 U1 Y3 V1
 *    first pixel:  Y0 U0 V0
 *    second pixel: Y1 U0 V0
 *    third pixel:  Y2 U1 V1
 *    forth pixel:  Y3 U1 V1
 *  every two adjacent pixels owes different Y, but share the same U and V.
 *  so Y jumps by every 2 bytes, U and V jumps by every 4 bytes;
 *  When Y jump 2 times, U and V jumps one time.
 *
 *  YUYV422(YCbCr) convert to RGB888:
 *      R = Y + 1.402 * (V - 128)
 *      G = Y - 0.34414 * (U - 128) - 0.71414 * (V - 128)
 *      B = Y + 1.772 * (U - 128)
 *  0 <= R <= 255, 0 <= G <= 255, 0 <= B <= 255;
 *
 */
int YUYV422toRGB888(unsigned char *yuyv, int width,
    int height, unsigned char *rgb, int length)
{
    int line, column;
    unsigned char *py, *pu, *pv;
    unsigned char *tmp = rgb;
    unsigned int counter = 0;
    unsigned char *ref = referenceYUYV;
    unsigned int index = 0;
    int DIFF = global.diff_pixels;
    int ABSY = global.diff_deviation;
    int ABSCbCr = global.diff_deviation;

    // assert about the length
    assert(length == width * height * 2);

    py = yuyv;
    pu = yuyv + 1;
    pv = yuyv + 3;

#define CLIP(x) ((x) >= 0xFF ? 0xFF : ((x) <= 0x00 ? 0x00 : (x)))

    for (line = 0; line < height; line++) {
        for (column = 0; column < width; column++) {
            *tmp++ = CLIP((double)*py + 1.402 * ((double) * pv - 128.0));
            *tmp++ = CLIP((double)*py - 0.34414 * ((double) * pu - 128.0)
                - 0.71414 * ((double) *pv - 128.0));
            *tmp++ = CLIP((double)*py + 1.772 * ((double) * pu - 128.0));

            // whether pixel changed
            if (column % 2 == 0) {
                if (flag == 2) {
                    dmd_log(LOG_INFO, "00000 py = %d, pu = %d, pv = %d\n",
                            *py, *pu, *pv);
                    dmd_log(LOG_INFO,
                            "00000 refpy = %d, refpu = %d, refpv = %d\n",
                            *(ref + index + 0),
                            *(ref + index + 1),
                            *(ref + index + 3));
                }

                int absy = abs(*(ref + index + 0) - *py);
                int absu = abs(*(ref + index + 1) - *pu);
                int absv = abs(*(ref + index + 3) - *pv);
                if (absy >= ABSY || absu >= ABSCbCr || absv >= ABSCbCr)
                    counter++;

                *(ref + index + 0) = *py;
            } else {
                if (flag == 2) {
                    dmd_log(LOG_INFO, "11111 py = %d, pu = %d, pv = %d\n",
                            *py, *pu, *pv);
                    dmd_log(LOG_INFO,
                            "11111 refpy = %d, refpu = %d, refpv = %d\n",
                            *(ref + index + 2),
                            *(ref + index + 1),
                            *(ref + index + 3));
                }

                int absy = abs(*(ref + index + 2) - *py);
                int absu = abs(*(ref + index + 1) - *pu);
                int absv = abs(*(ref + index + 3) - *pv);
                if (absy >= ABSY || absu >= ABSCbCr || absv >= ABSCbCr)
                    counter++;

                *(ref + index + 2) = *py;
            }

            py += 2; // jump to the adjcent pixel, or

            if (column % 2 == 1) { // jump to next u and v macro
                *(ref + index + 1) = *pu;
                *(ref + index + 3) = *pv;

                pu += 4;
                pv += 4;
                index += 4;
            } // if
        } // for inner
    } // for outer

    memcpy(referenceYUYV, yuyv, length);
    flag = 1;
    if (counter >= DIFF) {
        dmd_log(LOG_INFO, "diff counter = %d, captured a picture.\n", counter);
        return 0;
    } else {
        return -1;
    }
}

/*
 * convert YUYV422 to RGB88 using integer operation, 
 * it's said this method is more efficient than float operation;
 */
int YUYV422toRGB888INT(unsigned char *yuyv, int width,
    int height, unsigned char *rgb, int length)
{
    int line, column;
    unsigned char *py, *pu, *pv;
    unsigned char *tmp = rgb;
    unsigned int counter = 0;
    unsigned char *ref = referenceYUYV;
    unsigned int index = 0;
    int DIFF = global.diff_pixels;
    int ABSY = global.diff_deviation;
    int ABSCbCr = global.diff_deviation;

    // assert about the length
    assert(length == width * height * 2);

    py = yuyv;
    pu = yuyv + 1;
    pv = yuyv + 3;

#define CLIP(x) ((x) >= 0xFF ? 0xFF : ((x) <= 0x00 ? 0x00 : (x)))

    for (line = 0; line < height; line++) {
        for (column = 0; column < width; column++) {
            int u = *pu - 128;
            int v = *pv - 128;
            int rdiff = v + ((v * 103) >> 8);
            int gdiff = ((u * 88) >> 8) + ((v * 183) >> 8);
            int bdiff = u + ((u * 198) >> 8);

            *tmp++ = CLIP(*py + rdiff);
            *tmp++ = CLIP(*py - gdiff);
            *tmp++ = CLIP(*py + bdiff);

            // whether pixel changed
            if (column % 2 == 0) {
                int absy = abs(*(ref + index + 0) - *py);
                int absu = abs(*(ref + index + 1) - *pu);
                int absv = abs(*(ref + index + 3) - *pv);
                if (absy >= ABSY || absu >= ABSCbCr || absv >= ABSCbCr)
                    counter++;

                *(ref + index + 0) = *py;
            } else {

                int absy = abs(*(ref + index + 2) - *py);
                int absu = abs(*(ref + index + 1) - *pu);
                int absv = abs(*(ref + index + 3) - *pv);
                if (absy >= ABSY || absu >= ABSCbCr || absv >= ABSCbCr)
                    counter++;

                *(ref + index + 2) = *py;
            }

            py += 2; // jump to the adjcent pixel, or

            if (column % 2 == 1) { // jump to next u and v macro
                *(ref + index + 1) = *pu;
                *(ref + index + 3) = *pv;

                pu += 4;
                pv += 4;
                index += 4;
            } // if


        } // for inner
    } // for outer

    memcpy(referenceYUYV, yuyv, length);
    flag = 1;
    if (counter >= DIFF) {
        dmd_log(LOG_INFO, "diff counter = %d, captured a picture.\n", counter);
        return 0;
    } else {
        return -1;
    }
}

// convert packed YUYV422 to planar YUV422P
int YUYV422toYUV422P(unsigned char *yuyv422, int width,
	int height, unsigned char *yuv422p, int length)
{
    int line, column;
    unsigned char *py, *pu, *pv;
    unsigned char *yuyvTemp;

    // assert about the length
    assert(length == width * height * 2);

    yuyvTemp = yuyv422;
    py = yuv422p;
    pu = yuv422p + width * height;
    pv = yuv422p + (unsigned int)(width * height * 1.5);

    for (line = 0; line < height; line++) {
        for (column = 0; column < width * 2; column += 4) {
            *py++ = *yuyvTemp++;
            *pu++ = *yuyvTemp++;
            *py++ = *yuyvTemp++;
            *pv++ = *yuyvTemp++;
        }
    }

    assert(pv - pu == width * height * 0.5);
    assert(pu - yuv422p == width * height * 1.5);
    assert(pv - yuv422p == width * height * 2);
    assert(yuyvTemp - yuyv422 == width * height * 2);

    return  0;
}

// convert planar YUV422P to planar YUV420P
int YUV422PtoYUV420P(unsigned char *yuv422p, int width,
	int height, unsigned char *yuv420p, int length)
{
    int line, column;
    unsigned char *py, *pu, *pv;
    unsigned char *yuyvTemp;

    // assert about the length
    assert(length == width * height * 1.5);

    yuyvTemp = yuv422p;
    py = yuv420p;
    pu = yuv420p + width * height;
    pv = yuv420p + (unsigned int)(width * height * 1.25);

    for (line = 0; line < height; line += 2) {
        for (column = 0; column < width; column += 2) {
            *(py + line * width + column) =
                *(yuyvTemp + line * width + column);
            *(py + (line + 1) * width + column) =
                *(yuyvTemp + (line + 1)* width + column);

            *(py + line * width + column + 1) =
                *(yuyvTemp + line * width + column + 1);
            *(py + (line + 1) * width + column + 1) =
                *(yuyvTemp + (line + 1)* width + column + 1);

            *(pu + width * height + (line / 2) * width + column) =
                (*(yuyvTemp + width * height + (line / 2) * width + column) +
                *(yuyvTemp + width * height + (line / 2 + 1) * width + column))
                / 2;

            *(pv + width * height + (line / 2) * width + column) =
                (*(yuyvTemp + (unsigned int)(width * height * 1.5) +
                   (line / 2) * width + column) +
                 *(yuyvTemp + (unsigned int)(width * height * 1.5) +
                     (line / 2 + 1) * width + column)) / 2;
        }
    }

    return  0;

}

// convert packed YUYV422 to planar YUV420P
int YUYV422toYUV420P(unsigned char *yuyv422, int width,
	int height, unsigned char *yuv420p, int length)
{
    int i,j;
    unsigned char *pY = yuv420p;
    unsigned char *pU = yuv420p + width * height;
    unsigned char *pV = pU + (width * height) / 4;

    unsigned char *pYUVTemp = yuyv422;
    unsigned char *pYUVTempNext = yuyv422+width * 2;

    for (i = 0; i < height; i += 2) {
        for (j = 0; j < width; j += 2) {
            pY[j] = *pYUVTemp++;
            pY[j + width] = *pYUVTempNext++;

            pU[j / 2] =(*(pYUVTemp) + *(pYUVTempNext)) / 2;

            pYUVTemp++;
            pYUVTempNext++;


            pY[j + 1] = *pYUVTemp++;
            pY[j + 1 + width] = *pYUVTempNext++;


            pV[j / 2] =(*(pYUVTemp) + *(pYUVTempNext)) / 2;

            pYUVTemp++;
            pYUVTempNext++;
        }

        pYUVTemp += width * 2;
        pYUVTempNext += width * 2;
        pY += width * 2;
        pU += width / 2;
        pV += width / 2;
    }
    
    return 0;
}

