/*
 * Tiny Code Generator for QEMU
 *
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

#include "src/jit/arm/arm_assembler.h"

static const int jit_target_reg_alloc_order[] = {
    JIT_REG_R4,
    JIT_REG_R5,
    JIT_REG_R6,
    JIT_REG_R7,
    JIT_REG_R8,
    JIT_REG_R9,
    JIT_REG_R10,
    JIT_REG_R11,
    JIT_REG_R13,
    JIT_REG_R0,
    JIT_REG_R1,
    JIT_REG_R2,
    JIT_REG_R3,
    JIT_REG_R12,
    JIT_REG_R14,
};

static const int jit_target_call_iarg_regs[4] = {
    JIT_REG_R0, JIT_REG_R1, JIT_REG_R2, JIT_REG_R3
};
static const int jit_target_call_oarg_regs[2] = {
    JIT_REG_R0, JIT_REG_R1
};

#define TO_CPSR (1 << 20)

typedef enum {
    ARITH_AND = 0x0 << 21,
    ARITH_EOR = 0x1 << 21,
    ARITH_SUB = 0x2 << 21,
    ARITH_RSB = 0x3 << 21,
    ARITH_ADD = 0x4 << 21,
    ARITH_ADC = 0x5 << 21,
    ARITH_SBC = 0x6 << 21,
    ARITH_RSC = 0x7 << 21,
    ARITH_TST = 0x8 << 21 | TO_CPSR,
    ARITH_CMP = 0xa << 21 | TO_CPSR,
    ARITH_CMN = 0xb << 21 | TO_CPSR,
    ARITH_ORR = 0xc << 21,
    ARITH_MOV = 0xd << 21,
    ARITH_BIC = 0xe << 21,
    ARITH_MVN = 0xf << 21,

    INSN_LDR_IMM   = 0x04100000,
    INSN_LDR_REG   = 0x06100000,
    INSN_STR_IMM   = 0x04000000,
    INSN_STR_REG   = 0x06000000,

    INSN_LDRH_IMM  = 0x005000b0,
    INSN_LDRH_REG  = 0x001000b0,
    INSN_LDRSH_IMM = 0x005000f0,
    INSN_LDRSH_REG = 0x001000f0,
    INSN_STRH_IMM  = 0x004000b0,
    INSN_STRH_REG  = 0x000000b0,

    INSN_LDRB_IMM  = 0x04500000,
    INSN_LDRB_REG  = 0x06500000,
    INSN_LDRSB_IMM = 0x005000d0,
    INSN_LDRSB_REG = 0x001000d0,
    INSN_STRB_IMM  = 0x04400000,
    INSN_STRB_REG  = 0x06400000,

    INSN_LDRD_IMM  = 0x004000d0,
    INSN_LDRD_REG  = 0x000000d0,
    INSN_STRD_IMM  = 0x004000f0,
    INSN_STRD_REG  = 0x000000f0,
} ARMInsn;


static jit_insn_unit *tb_ret_addr;


/* Compute frame size via macros, to share between tcg_target_qemu_prologue
   and tcg_register_jit.  */

#define PUSH_SIZE  ((11 - 4 + 1 + 1) * sizeof(jit_target_long))

#define FRAME_SIZE \
    ((PUSH_SIZE \
      + JIT_STATIC_CALL_ARGS_SIZE \
      + CPU_TEMP_BUF_NLONGS * sizeof(long) \
      + JIT_TARGET_STACK_ALIGN - 1) \
     & -JIT_TARGET_STACK_ALIGN)

void jit_arm_prologue(JITContext *s) {
    int stack_addend;

    /* Calling convention requires us to save r4-r11 and lr.  */
    /* stmdb sp!, { r4 - r11, lr } */
    // jit_out32(s, (COND_AL << 28) | 0x092d4ff0);

    /* Reserve callee argument and tcg temp space.  */
    stack_addend = FRAME_SIZE - PUSH_SIZE;

    // tcg_out_dat_rI(s, COND_AL, ARITH_SUB, TCG_REG_CALL_STACK,
    //                TCG_REG_CALL_STACK, stack_addend, 1);
    // tcg_set_frame(s, TCG_REG_CALL_STACK, TCG_STATIC_CALL_ARGS_SIZE,
    //               CPU_TEMP_BUF_NLONGS * sizeof(long));

    // tcg_out_mov(s, TCG_TYPE_PTR, TCG_AREG0, tcg_target_call_iarg_regs[0]);

    // tcg_out_bx(s, COND_AL, tcg_target_call_iarg_regs[1]);
    // tb_ret_addr = s->code_ptr;

    // /* Epilogue.  We branch here via tb_ret_addr.  */
    // tcg_out_dat_rI(s, COND_AL, ARITH_ADD, TCG_REG_CALL_STACK,
    //                TCG_REG_CALL_STACK, stack_addend, 1);

    // /* ldmia sp!, { r4 - r11, pc } */
    // tcg_out32(s, (COND_AL << 28) | 0x08bd8ff0);
}


