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
 * File: x86_64_assembler.c
 *
 * Brief: implementation file of x86_64 jit;
 *
 * Date: 2014.09.14
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "src/jit/x86_64/x86_64_assembler.h"

static const int jit_target_reg_alloc_order[] = {
    JIT_REG_RBP,
    JIT_REG_RBX,
    JIT_REG_R12,
    JIT_REG_R13,
    JIT_REG_R14,
    JIT_REG_R15,
    JIT_REG_R10,
    JIT_REG_R11,
    JIT_REG_R9,
    JIT_REG_R8,
    JIT_REG_RCX,
    JIT_REG_RDX,
    JIT_REG_RSI,
    JIT_REG_RDI,
    JIT_REG_RAX,
};

// first six parameters are in registers;
static const int jit_target_call_iarg_regs[] = {
    JIT_REG_RDI,
    JIT_REG_RSI,
    JIT_REG_RDX,
    JIT_REG_RCX,
    JIT_REG_R8,
    JIT_REG_R9,
};

// return value is in register eax;
static const int jit_target_call_oarg_regs[] = {
    JIT_REG_EAX,
};

/* Registers used with L constraint, which are the first argument 
   registers on x86_64, and two random call clobbered registers on
   i386. */
#define TCG_REG_L0 tcg_target_call_iarg_regs[0]
#define TCG_REG_L1 tcg_target_call_iarg_regs[1]

static uint8_t *jit_ret_addr;

static int jit_callee_save_regs[] = {
    JIT_REG_RBP,
    JIT_REG_RBX,
    JIT_REG_R12,
    JIT_REG_R13,
    JIT_REG_R14, /* Currently used for the global env. */
    JIT_REG_R15,
};

static inline void jit_out_push(JITContext *s, int reg) {
    // tcg_out_opc(s, OPC_PUSH_r32 + LOWREGMASK(reg), 0, reg, 0);
}

static inline void jit_out_mov(JITContext *s, JITType type,
                               JITReg ret, JITReg arg) {
    // if (arg != ret) {
    //     int opc = OPC_MOVL_GvEv + (type == TCG_TYPE_I64 ? P_REXW : 0);
    //     tcg_out_modrm(s, opc, ret, arg);
    // }
}


/* Compute frame size via macros, and tcg_register_jit. */


#define PUSH_SIZE \
    ((1 + ARRAY_SIZE(jit_callee_save_regs)) * (JIT_TARGET_REG_BITS / 8))

#define FRAME_SIZE \
    ((PUSH_SIZE \
      + JIT_STATIC_CALL_ARGS_SIZE \
      + CPU_TEMP_BUF_NLONGS * sizeof(long) \
      + JIT_TARGET_STACK_ALIGN - 1) \
     & ~(JIT_TARGET_STACK_ALIGN - 1))

/* Generate global jit prologue and epilogue code */
void jit_x86_64_prologue(JITContext *s) {
    int i, stack_addend;

    /* TB prologue */

    /* Reserve some stack space */
    stack_addend = FRAME_SIZE - PUSH_SIZE;

    /* Save all callee saved registers.  */
    for (i = 0; i < ARRAY_SIZE(jit_callee_save_regs); i++) {
        // jit_out_push(s, jit_target_callee_save_regs[i]);
    }

    // jit_out_mov(s, JIT_TYPE_PTR, JIT_AREG0, jit_target_call_iarg_regs[0]);
    // jit_out_addi(s, JIT_REG_ESP, -stack_addend);
    /* jmp *tb.  */
    // jit_out_mod(s, OPC_GRP5, EXT5_JMPN_Ev, jit_target_call_iarg_regs[1]);

    /* TB epilogue */
    // jit_ret_addr = s->code_ptr;
    jit_ret_addr = s->code_ptr + stack_addend;

    // jit_out_addi(s, JIT_REG_CALL_STACK, stack_addend);

    for (i = ARRAY_SIZE(jit_callee_save_regs) - 1; i >= 0; i--) {
        // jit_out_pop(s, tcg_target_callee_save_regs[i]);
    }
    // jit_out_opc(s, OPC_RET, 0, 0, 0);
}

