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
 * File: jit_test.c
 *
 * Brief: test_file of jit;
 *
 * Date: 2014.09.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "src/jit/jit.h"
#include "src/global_context.h"

#if 0
# define tcg_qemu_tb_exec(env, tb_ptr) \
    ((uintptr_t (*)(void *, void *))tcg_ctx.code_gen_prologue)(env, tb_ptr)
#endif

static void jit_YUYV422_motion_detect(JITContext *s, BodyParams param) {
    struct motion_detect_param  detect = param.u.detect;
    printf("detect message:%p\n", &detect);

    // int DIFF = global.client.diff_pixels;
    // int ABSY = global.client.diff_deviation;
    // int ABSCbCr = global.client.diff_deviation;
    int client_offset = offsetof(struct global_context, client);
    int diff_pixels_offset = offsetof(struct client_context, diff_pixels);
    printf("client_offset = %d \n", client_offset);
    printf("diff_pixels_offset = %d \n", diff_pixels_offset);

    // ATTENTION: remember to type conversion &global to (uint8 *);
    int diff =  *(int *)((uint8_t *)&global + client_offset
            + diff_pixels_offset);
    int global_diff = global.client.diff_pixels;
    printf("diff = %d, global_diff = %d\n", diff, global_diff);
}

int main(int argc, char *argv[]) {
    struct add_param add = {1, 2};
    BodyParams param;
    param.u.add = add;

    jit_ctx = jit_init();
    jit_prologue(jit_ctx);
    jit_body(jit_ctx, ADD_TWO, param);
    int ret = jit_exec(jit_ctx, jit_ctx->code_gen_body);
    assert(ret == 3);
    jit_release(jit_ctx);

    init_default_global();
    jit_YUYV422_motion_detect(jit_ctx, param);

    return 0;
}

