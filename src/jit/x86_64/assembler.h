/*
 * Tiny Code Generator for QEMU
 *
 * Copyright (c) 2008 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef SRC_JIT_X86_64_ASSEMBLER_H_
#define SRC_JIT_X86_64_ASSEMBLER_H_

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

#endif  // SRC_JIT_X86_64_ASSEMBLER_H_

