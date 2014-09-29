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

#include <assert.h>
#include <stdint.h>

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
#define TCG_REG_L0 jit_target_call_iarg_regs[0]
#define TCG_REG_L1 jit_target_call_iarg_regs[1]

static uint8_t *jit_ret_addr;

static int jit_target_callee_save_regs[] = {
    JIT_REG_RBP,
    JIT_REG_RBX,
    JIT_REG_R12,
    JIT_REG_R13,
    JIT_REG_R14, /* Currently used for the global env. */
    JIT_REG_R15,
};

# define LOWREGMASK(x)  ((x) & 7)

#define P_EXT           0x100           /* 0x0f opcode prefix */
#define P_EXT38         0x200           /* 0x0f 0x38 opcode prefix */
#define P_DATA16        0x400           /* 0x66 opcode prefix */

// x86_64 specific
# define P_ADDR32       0x800           /* 0x67 opcode prefix */
# define P_REXW         0x1000          /* Set REX.W = 1 */
# define P_REXB_R       0x2000          /* REG field as byte register */
# define P_REXB_RM      0x4000          /* R/M field as byte register */
# define P_GS           0x8000          /* gs segment override */


#define P_SIMDF3        0x10000         /* 0xf3 opcode prefix */
#define P_SIMDF2        0x20000         /* 0xf2 opcode prefix */

#define OPC_ARITH_EvIz    (0x81)
#define OPC_ARITH_EvIb    (0x83)
#define OPC_ARITH_GvEv    (0x03)  /* ... plus (ARITH_FOO << 3) */
#define OPC_ANDN          (0xf2 | P_EXT38)
#define OPC_ADD_GvEv      (OPC_ARITH_GvEv | (ARITH_ADD << 3))
#define OPC_BSWAP         (0xc8 | P_EXT)
#define OPC_CALL_Jz       (0xe8)
#define OPC_CMOVCC        (0x40 | P_EXT)  /* ... plus condition code */
#define OPC_CMP_GvEv      (OPC_ARITH_GvEv | (ARITH_CMP << 3))
#define OPC_DEC_r32       (0x48)
#define OPC_IMUL_GvEv     (0xaf | P_EXT)
#define OPC_IMUL_GvEvIb   (0x6b)
#define OPC_IMUL_GvEvIz   (0x69)
#define OPC_INC_r32       (0x40)
#define OPC_JCC_long      (0x80 | P_EXT)  /* ... plus condition code */
#define OPC_JCC_short     (0x70)  /* ... plus condition code */
#define OPC_JMP_long      (0xe9)
#define OPC_JMP_short     (0xeb)
#define OPC_LEA           (0x8d)
#define OPC_MOVB_EvGv     (0x88)  /* stores, more or less */
#define OPC_MOVL_EvGv     (0x89)  /* stores, more or less */
#define OPC_MOVL_GvEv     (0x8b)  /* loads, more or less */
#define OPC_MOVB_EvIz     (0xc6)
#define OPC_MOVL_EvIz     (0xc7)
#define OPC_MOVL_Iv       (0xb8)
#define OPC_MOVBE_GyMy    (0xf0 | P_EXT38)
#define OPC_MOVBE_MyGy    (0xf1 | P_EXT38)
#define OPC_MOVSBL        (0xbe | P_EXT)
#define OPC_MOVSWL        (0xbf | P_EXT)
#define OPC_MOVSLQ        (0x63 | P_REXW)
#define OPC_MOVZBL        (0xb6 | P_EXT)
#define OPC_MOVZWL        (0xb7 | P_EXT)
#define OPC_POP_r32       (0x58)
#define OPC_PUSH_r32      (0x50)
#define OPC_PUSH_Iv       (0x68)
#define OPC_PUSH_Ib       (0x6a)
#define OPC_RET           (0xc3)
#define OPC_SETCC         (0x90 | P_EXT | P_REXB_RM)  /* ... plus cc */
#define OPC_SHIFT_1       (0xd1)
#define OPC_SHIFT_Ib      (0xc1)
#define OPC_SHIFT_cl      (0xd3)
#define OPC_SARX          (0xf7 | P_EXT38 | P_SIMDF3)
#define OPC_SHLX          (0xf7 | P_EXT38 | P_DATA16)
#define OPC_SHRX          (0xf7 | P_EXT38 | P_SIMDF2)
#define OPC_TESTL         (0x85)
#define OPC_XCHG_ax_r32   (0x90)

#define OPC_GRP3_Ev       (0xf7)
#define OPC_GRP5          (0xff)

/* Group 1 opcode extensions for 0x80-0x83.
 * These are also used as modifiers for OPC_ARITH.
 */
#define ARITH_ADD 0
#define ARITH_OR  1
#define ARITH_ADC 2
#define ARITH_SBB 3
#define ARITH_AND 4
#define ARITH_SUB 5
#define ARITH_XOR 6
#define ARITH_CMP 7

/* Group 2 opcode extensions for 0xc0, 0xc1, 0xd0-0xd3.  */
#define SHIFT_ROL 0
#define SHIFT_ROR 1
#define SHIFT_SHL 4
#define SHIFT_SHR 5
#define SHIFT_SAR 7

/* Group 3 opcode extensions for 0xf6, 0xf7.  To be used with OPC_GRP3.  */
#define EXT3_NOT   2
#define EXT3_NEG   3
#define EXT3_MUL   4
#define EXT3_IMUL  5
#define EXT3_DIV   6
#define EXT3_IDIV  7

/* Group 5 opcode extensions for 0xff.  To be used with OPC_GRP5.  */
#define EXT5_INC_Ev    0
#define EXT5_DEC_Ev    1
#define EXT5_CALLN_Ev  2
#define EXT5_JMPN_Ev   4

/* Condition codes to be added to OPC_JCC_{long,short}.  */
#define JCC_JMP (-1)
#define JCC_JO  0x0
#define JCC_JNO 0x1
#define JCC_JB  0x2
#define JCC_JAE 0x3
#define JCC_JE  0x4
#define JCC_JNE 0x5
#define JCC_JBE 0x6
#define JCC_JA  0x7
#define JCC_JS  0x8
#define JCC_JNS 0x9
#define JCC_JP  0xa
#define JCC_JNP 0xb
#define JCC_JL  0xc
#define JCC_JGE 0xd
#define JCC_JLE 0xe
#define JCC_JG  0xf


static inline void jit_out8(JITContext *s, uint8_t v) {
    *s->code_ptr++ = v;
}

static void jit_out_opc(JITContext *s, int opc, int r, int rm, int x) {
    int rex;

    if (opc & P_GS) {
        jit_out8(s, 0x65);
    }
    if (opc & P_DATA16) {
        /* We should never be asking for both 16 and 64-bit operation.  */
        assert((opc & P_REXW) == 0);
        jit_out8(s, 0x66);
    }
    if (opc & P_ADDR32) {
        jit_out8(s, 0x67);
    }

    rex = 0;
    rex |= (opc & P_REXW) ? 0x8 : 0x0;  /* REX.W */
    rex |= (r & 8) >> 1;                /* REX.R */
    rex |= (x & 8) >> 2;                /* REX.X */
    rex |= (rm & 8) >> 3;               /* REX.B */

    /* P_REXB_{R,RM} indicates that the given register is the low byte.
       For %[abcd]l we need no REX prefix, but for %{si,di,bp,sp}l we do,
       as otherwise the encoding indicates %[abcd]h.  Note that the values
       that are ORed in merely indicate that the REX byte must be present;
       those bits get discarded in output.  */
    rex |= opc & (r >= 4 ? P_REXB_R : 0);
    rex |= opc & (rm >= 4 ? P_REXB_RM : 0);

    if (rex) {
        jit_out8(s, (uint8_t)(rex | 0x40));
    }

    if (opc & (P_EXT | P_EXT38)) {
        jit_out8(s, 0x0f);
        if (opc & P_EXT38) {
            jit_out8(s, 0x38);
        }
    }

    jit_out8(s, opc);
}

static inline void jit_out_push(JITContext *s, int reg) {
    jit_out_opc(s, OPC_PUSH_r32 + LOWREGMASK(reg), 0, reg, 0);
}

static inline void jit_out_pop(JITContext *s, int reg) {
    jit_out_opc(s, OPC_POP_r32 + LOWREGMASK(reg), 0, reg, 0);
}

static void jit_out_modrm(JITContext *s, int opc, int r, int rm) {
    jit_out_opc(s, opc, r, rm, 0);
    jit_out8(s, 0xc0 | (LOWREGMASK(r) << 3) | LOWREGMASK(rm));
}

static inline void jit_out_mov(JITContext *s, JITType type,
        JITReg ret, JITReg arg) {
    if (arg != ret) {
        int opc = OPC_MOVL_GvEv + (type == JIT_TYPE_I64 ? P_REXW : 0);
        jit_out_modrm(s, opc, ret, arg);
    }
}

/* Compute frame size via macros, and tcg_register_jit. */

#define PUSH_SIZE \
    ((1 + ARRAY_SIZE(jit_target_callee_save_regs)) * (JIT_TARGET_REG_BITS / 8))

#define FRAME_SIZE \
    ((PUSH_SIZE \
      + JIT_STATIC_CALL_ARGS_SIZE \
      + CPU_TEMP_BUF_NLONGS * sizeof(long) \
      + JIT_TARGET_STACK_ALIGN - 1) \
     & ~(JIT_TARGET_STACK_ALIGN - 1))

/* Generate global jit prologue and epilogue code */
static void jit_x86_64_prologue(JITContext *s) {
    int i, stack_addend;

    /* TB prologue */

    /* Reserve some stack space, also for TCG temps.  */
    stack_addend = FRAME_SIZE - PUSH_SIZE;
    jit_set_frame(s, JIT_REG_CALL_STACK, JIT_STATIC_CALL_ARGS_SIZE,
                  CPU_TEMP_BUF_NLONGS * sizeof(long));

    /* Save all callee saved registers.  */
    for (i = 0; i < ARRAY_SIZE(jit_target_callee_save_regs); i++) {
        jit_out_push(s, jit_target_callee_save_regs[i]);
    }

    jit_out_mov(s, JIT_TYPE_PTR, JIT_AREG0, jit_target_call_iarg_regs[0]);
    // jit_out_addi(s, JIT_REG_ESP, -stack_addend);

    /* jmp *tb.  */
    jit_out_modrm(s, OPC_GRP5, EXT5_JMPN_Ev, jit_target_call_iarg_regs[1]);

    /* TB epilogue */
    jit_ret_addr = s->code_ptr;

    // jit_out_addi(s, JIT_REG_CALL_STACK, stack_addend);

    for (i = ARRAY_SIZE(jit_target_callee_save_regs) - 1; i >= 0; i--) {
        jit_out_pop(s, jit_target_callee_save_regs[i]);
    }
    jit_out_opc(s, OPC_RET, 0, 0, 0);
}

