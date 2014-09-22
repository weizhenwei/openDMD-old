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

#include <assert.h>

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

#define SHIFT_IMM_LSL(im) (((im) << 7) | 0x00)
#define SHIFT_IMM_LSR(im) (((im) << 7) | 0x20)
#define SHIFT_IMM_ASR(im) (((im) << 7) | 0x40)
#define SHIFT_IMM_ROR(im) (((im) << 7) | 0x60)
#define SHIFT_REG_LSL(rs) (((rs) << 8) | 0x10)
#define SHIFT_REG_LSR(rs) (((rs) << 8) | 0x30)
#define SHIFT_REG_ASR(rs) (((rs) << 8) | 0x50)
#define SHIFT_REG_ROR(rs) (((rs) << 8) | 0x70)

enum arm_cond_code_e {
    COND_EQ = 0x0,
    COND_NE = 0x1,
    COND_CS = 0x2,  /* Unsigned greater or equal */
    COND_CC = 0x3,  /* Unsigned less than */
    COND_MI = 0x4,  /* Negative */
    COND_PL = 0x5,  /* Zero or greater */
    COND_VS = 0x6,  /* Overflow */
    COND_VC = 0x7,  /* No overflow */
    COND_HI = 0x8,  /* Unsigned greater than */
    COND_LS = 0x9,  /* Unsigned less or equal */
    COND_GE = 0xa,
    COND_LT = 0xb,
    COND_GT = 0xc,
    COND_LE = 0xd,
    COND_AL = 0xe,
};

#if 0
static const uint8_t tcg_cond_to_arm_cond[] = {
    [TCG_COND_EQ] = COND_EQ,
    [TCG_COND_NE] = COND_NE,
    [TCG_COND_LT] = COND_LT,
    [TCG_COND_GE] = COND_GE,
    [TCG_COND_LE] = COND_LE,
    [TCG_COND_GT] = COND_GT,
    /* unsigned */
    [TCG_COND_LTU] = COND_CC,
    [TCG_COND_GEU] = COND_CS,
    [TCG_COND_LEU] = COND_LS,
    [TCG_COND_GTU] = COND_HI,
};
#endif

static jit_insn_unit *tb_ret_addr;

/**
 * ctz32 - count trailing zeros in a 32-bit value.
 * @val: The value to search
 *
 * Returns 32 if the value is zero.  Note that the GCC builtin is
 * undefined if the value is zero.
 */
static inline int ctz32(uint32_t val) {
    /* Binary search for the trailing one bit.  */
    int cnt;

    cnt = 0;
    if (!(val & 0x0000FFFFUL)) {
        cnt += 16;
        val >>= 16;
    }
    if (!(val & 0x000000FFUL)) {
        cnt += 8;
        val >>= 8;
    }
    if (!(val & 0x0000000FUL)) {
        cnt += 4;
        val >>= 4;
    }
    if (!(val & 0x00000003UL)) {
        cnt += 2;
        val >>= 2;
    }
    if (!(val & 0x00000001UL)) {
        cnt++;
        val >>= 1;
    }
    if (!(val & 0x00000001UL)) {
        cnt++;
    }

    return cnt;
}

static inline uint32_t rotl(uint32_t val, int n) {
  return (val << n) | (val >> (32 - n));
}

/* ARM immediates for ALU instructions are made of an unsigned 8-bit
   right-rotated by an even amount between 0 and 30. */
static inline int encode_imm(uint32_t imm) {
    int shift;

    /* simple case, only lower bits */
    if ((imm & ~0xff) == 0)
        return 0;
    /* then try a simple even shift */
    shift = ctz32(imm) & ~1;
    if (((imm >> shift) & ~0xff) == 0)
        return 32 - shift;
    /* now try harder with rotations */
    if ((rotl(imm, 2) & ~0xff) == 0)
        return 2;
    if ((rotl(imm, 4) & ~0xff) == 0)
        return 4;
    if ((rotl(imm, 6) & ~0xff) == 0)
        return 6;
    /* imm can't be encoded */
    return -1;
}

static inline void jit_out_dat_imm(JITContext *s, int cond,
        int opc, int rd, int rn, int im) {
    jit_out32(s, (cond << 28) | (1 << 25) | opc |
                    (rn << 16) | (rd << 12) | im);
}

static inline void jit_out_dat_reg(JITContext *s, int cond,
        int opc, int rd, int rn, int rm, int shift) {
    jit_out32(s, (cond << 28) | (0 << 25) | opc |
                    (rn << 16) | (rd << 12) | shift | rm);
}

static inline void jit_out_dat_rI(JITContext *s, int cond, int opc, JITArg dst,
        JITArg lhs, JITArg rhs, int rhs_is_const) {
    /* Emit either the reg,imm or reg,reg form of a data-processing insn.
     * rhs must satisfy the "rI" constraint.
     */
    if (rhs_is_const) {
        int rot = encode_imm(rhs);
        assert(rot >= 0);
        jit_out_dat_imm(s, cond, opc, dst, lhs, rotl(rhs, rot) | (rot << 7));
    } else {
        jit_out_dat_reg(s, cond, opc, dst, lhs, rhs, SHIFT_IMM_LSL(0));
    }
}


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
    jit_out32(s, (COND_AL << 28) | 0x092d4ff0);

    /* Reserve callee argument and tcg temp space.  */
    stack_addend = FRAME_SIZE - PUSH_SIZE;

    jit_out_dat_rI(s, COND_AL, ARITH_SUB, JIT_REG_CALL_STACK,
            JIT_REG_CALL_STACK, stack_addend, 1);
    // tcg_set_frame(s, TCG_REG_CALL_STACK, TCG_STATIC_CALL_ARGS_SIZE,
    //         CPU_TEMP_BUF_NLONGS * sizeof(long));

    // tcg_out_mov(s, TCG_TYPE_PTR, TCG_AREG0, tcg_target_call_iarg_regs[0]);

    // tcg_out_bx(s, COND_AL, tcg_target_call_iarg_regs[1]);
    // tb_ret_addr = s->code_ptr;

    // /* Epilogue.  We branch here via tb_ret_addr.  */
    // tcg_out_dat_rI(s, COND_AL, ARITH_ADD, TCG_REG_CALL_STACK,
    //                TCG_REG_CALL_STACK, stack_addend, 1);

    // /* ldmia sp!, { r4 - r11, pc } */
    jit_out32(s, (COND_AL << 28) | 0x08bd8ff0);
}

void jit_arm_epilogue(JITContext *s) {
    int stack_addend;

    /* Calling convention requires us to save r4-r11 and lr.  */
    /* stmdb sp!, { r4 - r11, lr } */
    jit_out32(s, (COND_AL << 28) | 0x092d4ff0);

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
    jit_out32(s, (COND_AL << 28) | 0x08bd8ff0);
}

