#include "instruction.h"
#include "output.h"
#include "section.h"
#include <stdio.h>
#include <assert.h>

enum SRType {
    SRType_LSL,
    SRType_LSR,
    SRType_ASR,
    SRType_RRX,
    SRType_ROR
};


uint32_t
arm_expand_imm12(uint32_t imm12)
{
    uint32_t rotation = (imm12 >> 8) & 0xf;
    uint32_t abcdefgh = imm12 & 0xff;
    switch (rotation) {
    case 0: return abcdefgh;
    case 1: return (abcdefgh >> 2) + ((abcdefgh & 0x03) << 30);
    case 2: return (abcdefgh >> 4) + ((abcdefgh & 0x0f) << 28);
    case 3: return (abcdefgh >> 6) + ((abcdefgh & 0x3f) << 26);
    case 4: return                   ((abcdefgh & 0xff) << 24);
    default: return abcdefgh << ((16 - rotation) * 2);
    }
}

enum SRType
arm_decode_imm_type(uint32_t type, uint32_t imm5)
{
    if (0 == type)
        return SRType_LSL;
    else if (1 == type)
        return SRType_LSR;
    else if (2 == type)
        return SRType_ASR;
    else if (3 == type) {
        return (0 == imm5) ? SRType_RRX : SRType_ROR;
    }
    assert(0 && "should never reach this point");
    return SRType_LSL;
}

uint32_t
arm_decode_imm_shift(uint32_t type, uint32_t imm5)
{
    if (0 == type)
        return imm5;
    else if (1 == type || 2 == type)
        return (imm5 != 0) ? imm5 : 32;
    else if (3 == type)
        return (0 == imm5) ? 1 : imm5;
    assert(0 && "should never reach this point");
    return 0;
}

// ==================

void
p_push(uint32_t code)
{
    const uint32_t register_list = code & 0xffff;
    uint32_t mask = 1;
    for (int reg = 0; reg < 16; reg ++) {
        if (register_list & mask)
            emit_code("   push(%d);", reg);
        mask <<= 1;
    }
}

void
p_sub_immediate(uint32_t code)
{
    const uint32_t imm12 = code & 0xfff;
    const uint32_t imm32 = arm_expand_imm12(imm12);

    const uint32_t Rd = (code >> 12) & 0xf;
    const uint32_t Rn = (code >> 16) & 0xf;

    emit_code("   r%d = r%d - %d;", Rd, Rn, imm32);
    assert(Rd != 15);   // pc
    int setflags = (code >> 20) & 1;
    if (setflags) {
        assert(0);
    }
}

void
p_sub_register(uint32_t code)
{
    assert(0);
    (void)code;
    //~ const uint32_t imm5 = (code >> 7) & 0x1f;
    //~ const uint32_t type = (code >> 5) & 0x03;
    //~ const uint32_t Rd = (code >> 12) & 0x0f;
    //~ const uint32_t Rn = (code >> 16) & 0x0f;
    //~
    //~ assert(Rd != 15);
    //~ assert(Rd != 14);
//~
    //~ int setflags = (code >> 20) & 1;
    //~ if (setflags) {
        //~ assert(0);
    //~ }
}

void
p_sub_register_shifted_register(uint32_t code)
{
    assert(0);
    int setflags = (code >> 20) & 1;
    if (setflags) {
        assert(0);
    }
}

void
p_str_immediate(uint32_t code)
{
    const uint32_t index = !!(code & (1 << 24));
    const uint32_t add = !!(code & (1 << 23));
    const uint32_t wback = !index || (code & (1 << 21));

    const uint32_t Rn = (code >> 16) & 0xf;
    const uint32_t Rt = (code >> 12) & 0xf;
    const uint32_t imm12 = (code & 0xfff);
    const int32_t imm32 = add ? imm12 : -imm12;

    assert(Rt != 15);   // do not want stores into .text

    if (index && !wback) {
        emit_code("   store(r%d + %d, r%d);", Rn, imm32, Rt);
    } else if (index && wback) {
        emit_code("   r%d = r%d + %d;", Rn, Rn, imm32);
        emit_code("   store(r%d, r%d);", Rn, Rt);
    } else if (!index && wback) {
        emit_code("   store(r%d, r%d);", Rn, Rt);
        emit_code("   r%d = r%d + %d;", Rn, Rn, imm32);
    } else {
        emit_code("   store(r%d, r%d);", Rn, Rt);
    }
}

void
p_ldr_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = !!(code & (1 << 24));
    const uint32_t add = !!(code & (1 << 23));
    const uint32_t wback = !index && (code & (1 << 21));

    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rt = (code >> 12) & 0x0f;
    const uint32_t imm12 = code & 0xfff;
    const int32_t imm32 = add ? imm12 : -imm12;

    if (15 == Rt)   // ldr pc, <something>
        assert(0);

    if (15 == Rn) {
        if (wback) {
            assert(0 && "writeback in ldr with Rn = pc");
        }

        emit_code("   r%d = %d;", Rt, get_word_at(pc + 8 + (index ? imm32 : 0)));
    } else {
        if (index && !wback) {
            emit_code("   r%d = load(r%d + %d);", Rt, Rn, imm32);
        } else if (index && wback) {
            emit_code("   r%d = r%d + %d;", Rn, Rn, imm32);
            emit_code("   r%d = load(r%d);", Rt, Rn);
        } else if (!index && wback) {
            emit_code("   r%d = load(r%d);", Rt, Rn);
            emit_code("   r%d = r%d + %d;", Rn, Rn, imm32);
        } else {
            emit_code("   r%d = load(r%d);", Rt, Rn);
        }
    }
}

void
p_add_register(uint32_t pc, uint32_t code)
{
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rm = (code) & 0x0f;

    assert(Rm != 15);   // needs different handling
    assert(Rd != 15);   // this will be jump

    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t setflags = (code & (1<<20));
    const uint32_t shift = arm_decode_imm_shift(type, imm5);

    if (setflags)
        assert(0 && "setflags not implemented");

    switch (arm_decode_imm_type(type, imm5)) {
    case SRType_LSL:
        if (15 == Rn) {
            emit_code("   r%d = %d + (r%d << %d);", Rd, pc + 8, Rm, shift);
        } else {
            emit_code("   r%d = r%d + (r%d << %d);", Rd, Rn, Rm, shift);
        }
        break;
    default:
        assert(0 && "not implemented");
    }
}

void
p_add_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm32 = arm_expand_imm12(code & 0xfff);

    assert(Rd != 15);
    if (15 == Rn) {
        emit_code("    r%d = %d;", Rd, pc + 8 + imm32);
        const uint32_t result = pc + 8 + imm32;
        if (setflags) {
            emit_code("    ASPR.C = %d;", (result < imm32));
        }
    } else {
        if (setflags) {
            emit_code("    tmp = r%d + %d;", Rn, imm32);
            emit_code("    ASPR.C = (tmp < r%d);", Rn);
        }
        emit_code("    r%d = r%d + %d;", Rd, Rn, imm32);
    }

    if (setflags) {
        emit_code("    ASPR.N = r%d & 0x80000000;", Rd);
        emit_code("    ASPR.Z = (r%d == 0);", Rd);
        // TODO: V flag
    }
}

void
p_cmp_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t imm12 = code & 0xfff;
    const uint32_t imm32 = arm_expand_imm12(imm12);
    const uint32_t Rn = (code >> 16) & 0x0f;

    if (15 == Rn) {
        emit_code("   tmp = %d - %d;", pc + 8, imm32);
        assert(0);
    } else {
        emit_code("   tmp = r%d - %d;", Rn, imm32);
        emit_code("   APSR.C = (tmp > r%d);", Rn);
        emit_code("   ASPR.N = (tmp & 0x80000000);");
        emit_code("   ASPR.Z = (tmp == 0);");
        // TODO: overflow handling
    }
}

void
p_b(uint32_t pc, uint32_t code)
{
    const uint32_t imm24 = code & 0x00ffffff;
    const uint32_t imm32 = (imm24 & (1<<23)) ? ((0xff000000 | imm24) << 2) : (imm24 << 2);

    emit_code("    goto label_%04x;", imm32 + 8 + pc);
}

void
p_bl(uint32_t pc, uint32_t code)
{
    const uint32_t imm24 = code & 0x00ffffff;
    const uint32_t imm32 = (imm24 & (1<<23)) ? ((0xff000000 | imm24) << 2) : (imm24 << 2);

    emit_code("    func_%04x();", imm32 + 8 + pc);
}

void
p_mov_register(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t Rm = code & 0x0f;

    assert(Rd != 15);
    if (15 == Rm) {
        emit_code("    r%d = %d;", Rd, pc + 8);
    } else {
        emit_code("    r%d = r%d;", Rd, Rm);
    }

    if (setflags) {
        emit_code("    ASPR.N = (r%d & 0x80000000);", Rd);
        emit_code("    ASPR.Z = (r%d == 0);", Rd);
        // C and V are not changed
    }
}

void
p_mov_immediate(uint32_t pc, uint32_t code)
{
    (void)pc;
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm8 = code & 0xff;
    const uint32_t rotation = ((code >> 8) & 0xf) * 2;

    const uint32_t imm32 = (imm8 >> rotation) | (imm8 << (32 - rotation));
    const uint32_t carry = (rotation) ? !!(imm32 & 0x80000000) : 42;

    assert(Rd != 15);
    emit_code("    r%d = %d;", Rd, imm32);
    if (setflags) {
        emit_code("    ASPR.N = (r%d & 0x80000000);", Rd);
        emit_code("    ASPR.Z = (r%d == 0);", Rd);
        if (42 != carry)
            emit_code("    ASPR.C = %d;", carry);
        // V unchanged
    }
}

void
process_instruction(uint32_t pc)
{
    uint32_t code = get_word_at(pc);

    printf("process_instruction(0x%x)\n", code);
    emit_code("label_%04x:", pc);

    const uint32_t condition = (code >> 28) & 0x0f;
    switch (condition) {
    case 0b0000: emit_code("  if (ASPR.Z) {"); break;
    case 0b0001: emit_code("  if (!ASPR.Z) {"); break;
    case 0b0010: emit_code("  if (ASPR.C) {"); break;
    case 0b0011: emit_code("  if (!ASPR.C) {"); break;
    case 0b0100: emit_code("  if (ASPR.N) {"); break;
    case 0b0101: emit_code("  if (!ASPR.N) {"); break;
    case 0b0110: emit_code("  if (ASPR.V) {"); break;
    case 0b0111: emit_code("  if (!ASPR.V) {"); break;
    case 0b1000: emit_code("  if (ASPR.C && !ASPR.Z) {"); break;
    case 0b1001: emit_code("  if (!ASPR.C && ASPR.Z) {"); break;
    case 0b1010: emit_code("  if (!!ASPR.N == !!ASPR.V) {"); break;
    case 0b1011: emit_code("  if (!!ASPR.N != !!ASPR.V) {"); break;
    case 0b1100: emit_code("  if (!ASPR.Z && !!ASPR.N == !!ASPR.V) {"); break;
    case 0b1101: emit_code("  if (ASPR.Z && !!ASPR.N != !!ASPR.V) {"); break;
    case 0b1110: /* unconditional, no code emited */ break;
    default: assert(0 && "wierd condition"); break;
    }


    if ((code & 0x0fff0fff) == 0x052d0004) {
        printf("push (single)\n");
        assert(0 && "not implemented");
    } else if ((code & 0x0fff0000) == 0x092d0000) {
        p_push(code);
    } else if ((code & 0x0fe00000) == 0x02400000) {
        p_sub_immediate(code);
    } else if ((code & 0x0fe00010) == 0x00400000) {
        p_sub_register(code);
    } else if ((code & 0x0fe00090) == 0x00400010) {
        p_sub_register_shifted_register(code);
    } else if ((code & 0x0e500000) == 0x04000000) {
        p_str_immediate(code);
    } else if ((code & 0x0e500000) == 0x04100000) {
        p_ldr_immediate(pc, code);
    } else if ((code & 0x0fe00010) == 0x00800000) {
        p_add_register(pc, code);
    } else if ((code & 0x0fe00000) == 0x02800000) {
        p_add_immediate(pc, code);
    } else if ((code & 0x0ff00000) == 0x03500000) {
        p_cmp_immediate(pc, code);
    } else if ((code & 0x0f000000) == 0x0a000000) {
        p_b(pc, code);
    } else if ((code & 0x0f000000) == 0x0b000000) {
        p_bl(pc, code);
    } else if ((code & 0x0fe00ff0) == 0x01a00000) {
        p_mov_register(pc, code);
    } else if ((code & 0x0fe00000) == 0x03a00000) {
        p_mov_immediate(pc, code);
    } else {
        assert(0 && "instruction code not implemented");
    }

    if (condition != 0x0e)
        emit_code("  }");
}
