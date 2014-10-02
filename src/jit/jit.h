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
 * File: jit.h
 *
 * Brief: include file of jit
 *
 * Date: 2014.09.15
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef SRC_JIT_JIT_H_
#define SRC_JIT_JIT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__arm__)
#include "src/jit/arm/arm_assembler.h"
#elif defined(__x86_64__)
#include "src/jit/x86_64/x86_64_assembler.h"
#else
#error "jit unsupported architecture"
#endif

#if defined(__arm__)
typedef uint32_t jit_insn_unit;
typedef int32_t jit_target_long;
typedef uint32_t jit_target_ulong;
#elif defined(__x86_64__)
typedef uint8_t jit_insn_unit;
typedef int64_t jit_target_long;
typedef uint64_t jit_target_ulong;
#else
#error "jit unsupported architecture"
#endif

typedef jit_target_ulong JITArg;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define CPU_TEMP_BUF_NLONGS 128

#define JIT_STATIC_CALL_ARGS_SIZE 128

#define jit_abort() \
do {\
    fprintf(stderr, "%s:%d: tcg fatal error\n", __FILE__, __LINE__);\
    abort();\
} while (0)


// TODO(weizhenwei): may overflow here
#define CODE_PROBLOGUE_LEN 4096

typedef struct JITContext {
    /* goto_tb support */
    uint8_t *code_gen_prologue;
    uint8_t *code_gen_body;
    uint8_t *code_ptr;


    intptr_t frame_start;
    intptr_t frame_end;
    int frame_reg;
} JITContext;

extern JITContext *jit_ctx;

extern JITContext *jit_init();
extern void jit_release(JITContext *ctx);

typedef enum JITType {
    JIT_TYPE_I32,
    JIT_TYPE_I64,
    JIT_TYPE_COUNT, /* number of different types */

    /* An alias for the size of the host register.  */
    JIT_TYPE_REG = JIT_TYPE_I64,

    /* An alias for the size of the native pointer.  */
    JIT_TYPE_PTR = JIT_TYPE_I64,

    /* An alias for the size of the target "long", aka register.  */
    JIT_TYPE_TL = JIT_TYPE_I64,
} JITType;

typedef enum BodyType {
    ADD_TWO,
    YUYV422_TO_RGB888,
} BodyType;

typedef struct BodyParams {
    jit_target_long a;
    jit_target_long b;
} BodyParams;

extern void jit_out32(JITContext *s, uint32_t v);
extern void jit_out64(JITContext *s, uint64_t v);

// make memory executable;
extern void map_exec(void *start, uint64_t size);

extern void jit_set_frame(JITContext *s,
        int reg, intptr_t start, intptr_t size);


// TODO(weizhenwei): it's a litte wield here, which these functions should be
//                   defined at architecture-specific files;
#if defined(__arm__)
extern void jit_arm_prologue(JITContext *s);
extern void jit_arm_body(JITContext *s, BodyType body_type, BodyParams param);
#elif defined(__x86_64__)
extern void jit_x86_64_prologue(JITContext *s);
extern void jit_x86_64_body(JITContext *s,
        BodyType body_type, BodyParams param);
#else
#error "jit unsupported architecture"
#endif

// extern void jit_prologue(JITContext *s);
// extern void jit_body(JITContext *s, BodyType body_type,
//                      struct bodyParams param);
#define jit_prologue(s) jit_prologue_specific(s)
#define jit_body(s, body_type, param) jit_body_specific(s, body_type, param)

# define jit_exec(env, body_ptr) \
    ((int (*)(void *, void *))jit_ctx->code_gen_prologue)(env, body_ptr)

#endif  // SRC_JIT_JIT_H_
