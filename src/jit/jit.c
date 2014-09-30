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
 * File: jit.c
 *
 * Brief: implementation file of jit
 *
 * Date: 2014.09.15
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "src/jit/jit.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

JITContext *jitctx = NULL;

JITContext *jit_init() {
    JITContext *ctx = (JITContext *)malloc(sizeof(JITContext));
    assert(ctx != NULL);

    ctx->code_buf = (uint8_t *) malloc(4096);
    assert(ctx->code_buf != NULL);
    ctx->code_ptr = ctx->code_buf;

    ctx->frame_start = 0;
    ctx->frame_end = 0;
    ctx->frame_reg = 0;

    return ctx;
}

void jit_release(JITContext *ctx) {
    if (ctx == NULL)
        return ;

    if (ctx->code_buf != NULL) {
        free(ctx->code_buf);
    }

    free(ctx);
}

void jit_out32(JITContext *s, uint32_t v) {
    *s->code_ptr++ = v;
    // if (JIT_TARGET_INSN_UNIT_SIZE == 4) {
    //     *s->code_ptr++ = v;
    // } else {
    //     jit_insn_unit *p = s->code_ptr;
    //     memcpy(p, &v, sizeof(v));
    //     s->code_ptr = p + (4 / TCG_TARGET_INSN_UNIT_SIZE);
    // }
}

void jit_set_frame(JITContext *s, int reg, intptr_t start, intptr_t size) {
    s->frame_start = start;
    s->frame_end = start + size;
    s->frame_reg = reg;
}

void jit_build(JITContext *s, BodyType body_type) {
    // jit_prologue(s);
    // jit_body(s, body_type);
    // jit_prologue(s);
}

