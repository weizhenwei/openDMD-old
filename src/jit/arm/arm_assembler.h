/*
 * Tiny Code Generator for QEMU
 *
 * Copyright (c) 2008 Fabrice Bellard
 * Copyright (c) 2008 Andrzej Zaborowski
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

#ifndef SRC_JIT_ARM_ARM_ASSEMBLER_H_
#define SRC_JIT_ARM_ARM_ASSEMBLER_H_

#define JIT_TARGET_INSN_UNIT_SIZE 4

#include "src/jit/jit.h"

typedef enum {
    JIT_REG_R0 = 0,
    JIT_REG_R1,
    JIT_REG_R2,
    JIT_REG_R3,
    JIT_REG_R4,
    JIT_REG_R5,
    JIT_REG_R6,
    JIT_REG_R7,
    JIT_REG_R8,
    JIT_REG_R9,
    JIT_REG_R10,
    JIT_REG_R11,
    JIT_REG_R12,
    JIT_REG_R13,
    JIT_REG_R14,
    JIT_REG_PC,
} JITReg;

#define JIT_TARGET_NB_REGS 16

/* used for function call generation */
#define JIT_REG_CALL_STACK JIT_REG_R13
#define JIT_TARGET_STACK_ALIGN 8
#define JIT_TARGET_CALL_ALIGN_ARGS 1
#define JIT_TARGET_CALL_STACK_OFFSET 0


// upper function calling interface;
#define jit_prologue_specific jit_arm_prologue
#define jit_body_specific jit_arm_body

enum {
    JIT_AREG0 = JIT_REG_R6,
};

#endif  // SRC_JIT_ARM_ARM_ASSEMBLER_H_
