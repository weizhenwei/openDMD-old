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
 * File: x86_64_assembler.h
 *
 * Brief: include file of x86_64 jit;
 *
 * Date: 2014.09.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef SRC_JIT_X86_64_X86_64_ASSEMBLER_H_
#define SRC_JIT_X86_64_X86_64_ASSEMBLER_H_

#include "src/jit/jit.h"

#define JIT_TARGET_INSN_UNIT_SIZE  1

# define JIT_TARGET_REG_BITS  64
# define JIT_TARGET_NB_REGS   16

typedef enum {
    JIT_REG_EAX = 0,
    JIT_REG_ECX,
    JIT_REG_EDX,
    JIT_REG_EBX,
    JIT_REG_ESP,
    JIT_REG_EBP,
    JIT_REG_ESI,
    JIT_REG_EDI,

    /* 64-bit registers; always define the symbols to avoid
       too much if-deffing.  */
    JIT_REG_R8,
    JIT_REG_R9,
    JIT_REG_R10,
    JIT_REG_R11,
    JIT_REG_R12,
    JIT_REG_R13,
    JIT_REG_R14,
    JIT_REG_R15,
    JIT_REG_RAX = JIT_REG_EAX,
    JIT_REG_RCX = JIT_REG_ECX,
    JIT_REG_RDX = JIT_REG_EDX,
    JIT_REG_RBX = JIT_REG_EBX,
    JIT_REG_RSP = JIT_REG_ESP,
    JIT_REG_RBP = JIT_REG_EBP,
    JIT_REG_RSI = JIT_REG_ESI,
    JIT_REG_RDI = JIT_REG_EDI,
} JITReg;

/* used for function call generation */
#define JIT_REG_CALL_STACK JIT_REG_ESP
#define JIT_TARGET_STACK_ALIGN 16
#define JIT_TARGET_CALL_STACK_OFFSET 0

# define JIT_AREG0 JIT_REG_R14

#define jit_prologue_specific jit_x86_64_prologue
#define jit_body_specific jit_x86_64_body
#define jit_epilogue_specific jit_x86_64_epilogue

#endif  // SRC_JIT_X86_64_X86_64_ASSEMBLER_H_

