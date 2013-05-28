#include "instruction.h"
#include "output.h"
#include "section.h"
#include "pc-stack.h"
#include "patterns.h"
#include <stdio.h>
#include <assert.h>

enum SRType {
    SRType_LSL,
    SRType_LSR,
    SRType_ASR,
    SRType_RRX,
    SRType_ROR
};

int flag_function_end_found = 0;
int flag_last_instruction_ended_with_return = 0;

void
begin_function(uint32_t pc)
{
    flag_function_end_found = 0;
    emit_code("static void func_%04x() {", pc);
}

int
function_end_found()
{
    return flag_function_end_found;
}

int
last_instruction_ended_with_return()
{
    return flag_last_instruction_ended_with_return;
}

void
set_function_end_flag()
{
    flag_function_end_found = 1;
    flag_last_instruction_ended_with_return = 1;
}

void
end_function()
{
    emit_code("}");
}

// ============================

uint32_t
arm_expand_imm12(uint32_t imm12)
{
    uint32_t rotation = 2 * ((imm12 >> 8) & 0xf);
    uint32_t imm8 = imm12 & 0xff;

    return (imm8 >> rotation) | (imm8 << (32 - rotation));
}

uint32_t
arm_sign_extend_imm24(uint32_t imm24)
{
    if (imm24 & 0x00800000)
        return imm24 | 0xff000000;
    else
        return imm24;
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
p_sub_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t imm12 = code & 0xfff;
    const uint32_t imm32 = arm_expand_imm12(imm12);

    const uint32_t Rd = (code >> 12) & 0xf;
    const uint32_t Rn = (code >> 16) & 0xf;

    assert(Rd != 15);   // pc

    if (15 == Rn) {
        emit_code("   r%u = %u;", Rd, pc + 8 - imm32);
    } else {
        emit_code("   r%u = r%u - %u;", Rd, Rn, imm32);
    }

    int setflags = (code >> 20) & 1;
    if (setflags) {
        assert(0);
    }

    pc_stack_push(pc + 4);
}

void
p_sub_register_shifted_register(uint32_t pc, uint32_t code)
{
    assert(0);
    int setflags = (code >> 20) & 1;
    if (setflags) {
        assert(0);
    }

    pc_stack_push(pc + 4);
}

void
p_str_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rn = (code >> 16) & 0xf;
    const uint32_t Rt = (code >> 12) & 0xf;
    const uint32_t imm12 = (code & 0xfff);
    const int32_t offset = add ? imm12 : -imm12;

    assert(Rn != 15);   // do not want stores into .text
    assert(Rt != 15);

    if (index && !wback) {
        emit_code("   store(r%u + %d, r%u);", Rn, offset, Rt);
    } else if (index && wback) {
        emit_code("   r%u += %d;", Rn, offset);
        emit_code("   store(r%u, r%u);", Rn, Rt);
    } else if (!index) {
        emit_code("   store(r%u, r%u);", Rn, Rt);
        emit_code("   r%u += %d;", Rn, offset);
    }

    pc_stack_push(pc + 4);
}

void
p_ldr_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));

    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rt = (code >> 12) & 0x0f;
    const uint32_t imm12 = code & 0xfff;
    const int32_t offset = add ? imm12 : -imm12;

    if (15 == Rt)   // ldr pc, <something>
        assert(0);

    if (15 == Rn) {
        if (wback) {
            assert(0 && "writeback in ldr with Rn = pc");
        }
        emit_code("   r%u = %u;", Rt, get_word_at(pc + 8 + (index ? offset : 0)));
    } else {
        if (index && !wback) {
            emit_code("   r%u = load(r%u + %d);", Rt, Rn, offset);
        } else if (index && wback) {
            emit_code("   r%u += %d;", Rn, offset);
            emit_code("   r%u = load(r%u);", Rt, Rn);
        } else if (!index) {
            emit_code("   r%u = load(r%u);", Rt, Rn);
            emit_code("   r%u += %d;", Rn, offset);
        }
    }

    pc_stack_push(pc + 4);
}

/** @brief does addition, subtraction with or without carry
*/
void
p_addsubcarry_register(uint32_t pc, uint32_t code, uint32_t do_add, uint32_t do_carry)
{
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rm = (code) & 0x0f;

    assert(Rm != 15);   // needs different handling

    if (Rd == 15) {
        // check for known patterns
        if (pattern_switch_case(pc)) {
            // code will be generated in `pattern_switch_case`
            return;
        } else {
            assert(0 && "unknown jump pattern");
        }
    }

    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t setflags = (code & (1<<20));
    const uint32_t shift = arm_decode_imm_shift(type, imm5);

    emit_code("    {");
    switch (arm_decode_imm_type(type, imm5)) {
    case SRType_LSL:
        if (do_carry && do_add)         emit_code("      uint32_t tmp = (r%u << %u) + APSR.C;", Rm, shift);
        else if (!do_carry && do_add)   emit_code("      uint32_t tmp = r%u << %u;", Rm, shift);
        else if (do_carry && !do_add)   emit_code("      uint32_t tmp = ~(r%u << %u) + APSR.C;", Rm, shift);
        else if (!do_carry && !do_add)  emit_code("      uint32_t tmp = ~(r%u << %u) + 1;", Rm, shift);
        break;
    case SRType_LSR:
        if (do_carry && do_add)         emit_code("      uint32_t tmp = (r%u >> %u) + APSR.C;", Rm, shift);
        else if (!do_carry && do_add)   emit_code("      uint32_t tmp = (r%u >> %u);", Rm, shift);
        else if (do_carry && !do_add)   emit_code("      uint32_t tmp = ~(r%u >> %u) + APSR.C;", Rm, shift);
        else if (!do_carry && !do_add)  emit_code("      uint32_t tmp = ~(r%u >> %u) + 1;", Rm, shift);
        break;
    default:
        assert(0 && "shift type not implemented");
    }
    if (15 == Rn) {
        emit_code("     r%u = %u + tmp;", Rd, pc + 8);
        if (setflags)
            assert(0 && "setflags not implemented for Rn == 15 case");
    } else {
        emit_code("     r%u = r%u + tmp;", Rd, Rn);
        if (setflags) {
            emit_code("      APSR.N = !!(r%u & 0x80000000);", Rd);
            emit_code("      APSR.Z = (0 == r%u);", Rd);
            emit_code("      APSR.C = (r%u < tmp);", Rd);
            emit_code("      APSR.V = !((r%u ^ tmp) & 0x80000000) && ((r%u ^ tmp) & 0x80000000);", Rn, Rd);
        }
    }

    emit_code("    }");

    pc_stack_push(pc + 4);
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
        emit_code("    r%u = %u;", Rd, pc + 8 + imm32);
        if (setflags) {
            const uint32_t result = pc + 8 + imm32;
            emit_code("    APSR.C = %u;", (result < imm32));
            emit_code("    APSR.V = %u;",
                !(((pc+8) ^ imm32) & 0x80000000) && ((result ^ imm32) & 0x80000000));
        }
    } else {
        if (setflags) {
            emit_code("    tmp = r%u + %u;", Rn, imm32);
            emit_code("    APSR.C = (tmp < r%u);", Rn);
            emit_code("    APSR.V = !((r%u ^ %u) & 0x80000000) && ((tmp ^ r%u) & 0x80000000);",
                Rn, imm32, Rn);
        }
        emit_code("    r%u = r%u + %u;", Rd, Rn, imm32);
    }

    if (setflags) {
        emit_code("    APSR.N = !!(r%u & 0x80000000);", Rd);
        emit_code("    APSR.Z = (0 == r%u);", Rd);
    }

    pc_stack_push(pc + 4);
}

void
p_cmp_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t imm32 = arm_expand_imm12(code & 0xfff);
    const uint32_t imm32rev = ~imm32 + 1;
    const uint32_t Rn = (code >> 16) & 0x0f;

    emit_code("    {");
    if (15 == Rn) {
        assert(0 && "Rn == 15 in cmp");
    } else {
        emit_code("      uint32_t tmp = r%u + %uu;", Rn, imm32rev);
        emit_code("      APSR.C = (tmp > r%u);", Rn);
        emit_code("      APSR.N = !!(tmp & 0x80000000);");
        emit_code("      APSR.Z = (0 == tmp);");
        emit_code("      APSR.V = !((r%u ^ %uu) & 0x80000000) && ((tmp ^ r%u) & 0x80000000);",
            Rn, imm32rev, Rn);
    }
    emit_code("    }");

    pc_stack_push(pc + 4);
}

void
p_b(uint32_t pc, uint32_t code)
{
    const uint32_t imm32 = arm_sign_extend_imm24(code & 0x00ffffff) << 2;

    emit_code("    goto label_%04x;", imm32 + 8 + pc);

    pc_stack_push(imm32 + 8 + pc);
    if (((code >> 28) & 0x0f) != 0x0e) {
        // if branch was conditional, continue to explore forward
        pc_stack_push(pc + 4);
    }
}

void
p_bl(uint32_t pc, uint32_t code)
{
    const uint32_t imm32 = arm_sign_extend_imm24(code & 0x00ffffff) << 2;

    emit_code("    func_%04x();", imm32 + 8 + pc);

    func_list_add(imm32 + 8 + pc);
    pc_stack_push(pc + 4);
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
        emit_code("    APSR.N = (r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        // C and V are not changed
    }

    pc_stack_push(pc + 4);
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
    emit_code("    r%u = %uu;", Rd, imm32);
    if (setflags) {
        emit_code("    APSR.N = (r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        if (42 != carry)
            emit_code("    APSR.C = %d;", carry);
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_ldrd_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rt1 = (code >> 12) & 0x0f;
    const uint32_t Rt2 = Rt1 + 1;
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t imm4H = (code >> 8) & 0xf;
    const uint32_t imm4L = (code & 0x0f);
    const uint32_t imm32 = (imm4H << 4) | imm4L;
    const int32_t offset = add ? imm32 : -imm32;

    assert((Rt1 & 1) == 0);
    assert(Rt2 < 15);

    if (15 == Rn) {
        if (wback)
            assert(0 && "writeback to pc");

        if (index) {
            emit_code("    r%d = %d;", Rt1, get_word_at(pc + 8 + offset));
            emit_code("    r%d = %d;", Rt2, get_word_at(pc + 8 + offset + 4));
        } else {
            emit_code("    r%d = %d;", Rt1, get_word_at(pc + 8));
            emit_code("    r%d = %d;", Rt2, get_word_at(pc + 8 + 4));
        }
    }
    if (index && !wback) {
        emit_code("    r%d = load(r%d + %d);", Rt1, Rn, offset);
        emit_code("    r%d = load(r%d + %d);", Rt2, Rn, offset + 4);
    } else if (index && wback) {
        emit_code("    r%d = r%d + %d;", Rn, Rn, offset);
        emit_code("    r%d = load(r%d);", Rt1, Rn);
        emit_code("    r%d = load(r%d + 4);", Rt2, Rn);
    } else if (!index && wback) {
        emit_code("    r%d = load(r%d);", Rt1, Rn);
        emit_code("    r%d = load(r%d + 4);", Rt2, Rn);
        emit_code("    r%d = r%d + %d;", Rn, Rn, offset);
    } else {
        emit_code("    r%d = load(r%d);", Rt1, Rn);
        emit_code("    r%d = load(r%d + 4);", Rt2, Rn);
    }

    pc_stack_push(pc + 4);
}

void
p_ldm(uint32_t pc, uint32_t code)
{
    (void)pc;
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t wback = code & (1 << 21);

    uint32_t mask = 1;
    uint32_t offset = 0;
    for (uint32_t k = 0; k <= 14; k ++) {
        if (code & mask) {
            assert(!(k == Rn && wback));
            emit_code("    r%d = load(r%d + %d);", k, Rn, offset);
            offset += 4;
        }
        mask = mask << 1;
    }

    if (code & (1<<15))
        offset += 4;
    if (wback)
        emit_code("    r%d = r%d + %d;", Rn, Rn, offset);
    if (code & (1<<15)) {
        set_function_end_flag();
        emit_code("    return;");
    } else {
        pc_stack_push(pc + 4);
    }
}

void
p_stmdb(uint32_t pc, uint32_t code)
{
    const uint32_t wback = code & (1 << 21);
    const uint32_t Rn = (code >> 16) & 0x0f;

    uint32_t mask = 1;
    uint32_t neg_offset = 0;
    for (uint32_t k = 0; k <= 15; k ++) {
        if (code & mask)
            neg_offset += 4;
        mask <<= 1;
    }
    uint32_t storage_size = neg_offset;

    mask = 1;
    for (uint32_t k = 0; k <= 14; k ++) {
        if (code & mask) {
            emit_code("    store(r%d - %d, r%d);", Rn, neg_offset, k);
            neg_offset -= 4;
        }
        mask <<= 1;
    }

    if (code & (1 << 15))
        emit_code("    store(r%d - %d, %d);", Rn, neg_offset, pc + 8);

    if (wback)
        emit_code("    r%d = r%d - %d;", Rn, Rn, storage_size);

    pc_stack_push(pc + 4);
}

void
p_strd_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rt1 = (code >> 12) & 0x0f;
    const uint32_t Rt2 = Rt1 + 1;
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t imm4H = (code >> 8) & 0x0f;
    const uint32_t imm4L = code & 0x0f;
    const uint32_t imm32 = (imm4H << 4) | imm4L;
    const int32_t offset = add ? imm32 : -imm32;

    assert((Rt1 & 1) == 0);
    assert(Rt1 != 15);
    assert(Rt2 != 15);

    if (index && !wback) {
        emit_code("    store(r%d + %d, r%d);", Rn, offset, Rt1);
        emit_code("    store(r%d + %d + 4, r%d);", Rn, offset, Rt2);
    } else if (index && wback) {
        emit_code("    r%d = r%d + %d;", Rn, Rn, offset);
        emit_code("    store(r%d, r%d);", Rn, Rt1);
        emit_code("    store(r%d + 4, r%d);", Rn, Rt2);
    } else if (!index && wback) {
        emit_code("    store(r%d, r%d);", Rn, Rt1);
        emit_code("    store(r%d + 4, r%d);", Rn, Rt2);
        emit_code("    r%d = r%d + %d;", Rn, Rn, offset);
    } else { // !index &&  !wback
        emit_code("    store(r%d, r%d);", Rn, Rt1);
        emit_code("    store(r%d + 4, r%d);", Rn, Rt2);
    }

    pc_stack_push(pc + 4);
}

void
p_cmp_register(uint32_t pc, uint32_t code)
{
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t Rm = code & 0x0f;
    const enum SRType shift_t = arm_decode_imm_type(type, imm5);
    const uint32_t shift_n = arm_decode_imm_shift(type, imm5);

    emit_code("    {");
    switch (shift_t) {
    case SRType_LSL:
        emit_code("      uint32_t rhs = ~(r%d << %d) + 1;", Rm, shift_n);
        break;
    case SRType_LSR:
        emit_code("      uint32_t rhs = ~(r%d >> %d) + 1;", Rm, shift_n);
        break;
    default:
        assert(0 && "not implemented");
    }
    emit_code("      tmp = r%d + rhs;", Rn);
    emit_code("      APSR.N = !!(tmp & 0x80000000);");
    emit_code("      APSR.Z = (tmp == 0);");
    emit_code("      APSR.C = (tmp < rhs);");
    emit_code("      APSR.V = !((r%d ^ rhs) & 0x80000000) && ((tmp ^ rhs) & 0x80000000);", Rn);
    emit_code("    }");

    pc_stack_push(pc + 4);
}

void
p_lsl_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t Rm = code & 0x0f;
    const uint32_t shift_n = arm_decode_imm_shift(0, imm5);

    assert(Rd != 15);
    assert(shift_n > 0);

    if (setflags)
        emit_code("    APSR.C = !!((r%d << %d) & 0x80000000);", Rm, shift_n - 1);

    emit_code("    r%d = r%d << %d;", Rd, Rm, shift_n);

    if (setflags) {
        emit_code("    APSR.N = !!(r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_and_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm12 = code & 0xfff;
    const uint32_t imm32 = arm_expand_imm12(imm12);

    assert(Rd != 15);
    if (15 == Rn) {
        emit_code("    r%d = %uu;", Rd, (pc + 8) & imm32);
    } else {
        emit_code("    r%d = r%d & %uu;", Rd, Rn, imm32);
    }

    if (setflags) {
        emit_code("    APSR.N = !!(r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        emit_code("    APSR.C = %d;", !!(imm32 & 0x80000000));
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_asr_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t Rm = code & 0x0f;
    const uint32_t shift_n = arm_decode_imm_shift(0b10, imm5);

    assert(Rd != 15);
    assert(shift_n > 0);

    if (15 == Rm) {
        emit_code("    r%d = %u;", Rd, (uint32_t)((int32_t)(pc+8) >> shift_n));
        if (setflags)
            emit_code("    APSR.C = %d;", (uint32_t)((int32_t)(pc+8) >> (shift_n - 1)) & 1);
    } else {
        emit_code("    r%d = (uint32_t)((int32_t)r%d >> %d);", Rd, Rm, shift_n);
        if (setflags)
            emit_code("    APSR.C = (uint32_t)((int32_t)r%d >> %d) & 1;", Rm, shift_n - 1);
    }

    if (setflags) {
        emit_code("    APSR.N = !!(r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_strh_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rt = (code >> 12) & 0x0f;
    const uint32_t imm4H = (code >> 8) & 0x0f;
    const uint32_t imm4L = code & 0x0f;
    const uint32_t imm32 = (imm4H << 4) | imm4L;
    const int32_t offset = add ? imm32 : -imm32;

    assert(Rt != 15);   // possible, but not needed now
    assert(Rn != 15);

    if (index && !wback) {
        emit_code("   store_halfword(r%d + %d, r%d & 0xffff);", Rn, offset, Rt);
    } else if (index && wback) {
        emit_code("   r%d = r%d + %d;", Rn, Rn, offset);
        emit_code("   store_halfword(r%d, r%d & 0xffff);", Rn, Rt);
    } else if (!index) {
        emit_code("   store_halfword(r%d, r%d & 0xffff);", Rn, Rt);
        if (wback)
            emit_code("   r%d = r%d + %d;", Rn, Rn, offset);
    }

    pc_stack_push(pc + 4);
}

void
p_orr_register(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t Rm = code & 0x0f;
    const enum SRType shift_t = arm_decode_imm_type(type, imm5);
    const uint32_t shift_n = arm_decode_imm_shift(type, imm5);

    assert(Rd != 15);
    assert(Rn != 15);
    assert(Rm != 15);

    switch (shift_t) {
    case SRType_LSL:
        // APSR.C will be changed only if setflags set and actual shift happens
        // otherwise it will be set to own value, i.e. unchanged
        if (setflags && (shift_n > 0))
            emit_code("    APSR.C = !!((r%d << %d) & 0x80000000);", Rm, shift_n - 1);
        emit_code("    r%d = r%d | (r%d << %d);", Rd, Rn, Rm, shift_n);
        break;
    case SRType_LSR:
        if (setflags && (shift_n > 0))
            emit_code("    APSR.C = !!((r%d >> %d) & 1);", Rm, shift_n - 1);
        emit_code("    r%d = r%d | (r%d >> %d);", Rd, Rn, Rm, shift_n);
        break;
    default:
        assert(0 && "not implemented shift type");
        break;
    }

    if (setflags) {
        emit_code("    APSR.N = !!(r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_mvn_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm12 = code & 0xfff;
    const uint32_t imm32 = arm_expand_imm12(imm12);

    assert(Rd != 15);

    emit_code("    r%u = %uu;", Rd, ~imm32);
    if (setflags) {
        emit_code("    APSR.N = !!(r%u & 0x80000000);", Rd);
        emit_code("    APSR.Z = (0 == r%u);", Rd);
        emit_code("    APSR.C = %d;", !!(imm32 & 0x80000000));  // TODO: may be wrong for zero shift
    }

    pc_stack_push(pc + 4);
}

void
p_rsb_register(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t Rm = code & 0x0f;
    const enum SRType shift_t = arm_decode_imm_type(type, imm5);
    const uint32_t shift_n = arm_decode_imm_shift(type, imm5);

    assert(Rd != 15);
    assert(Rn != 15);
    assert(Rm != 15);

    emit_code("    {");
    switch (shift_t) {
    case SRType_LSL:
        emit_code("      uint32_t rhs = (r%d << %d) + 1;", Rm, shift_n);
        break;
    case SRType_LSR:
        emit_code("      uint32_t rhs = (r%d >> %d) + 1;", Rm, shift_n);
        break;
    default:
        assert(0 && "not implemented");
        break;
    }

    if (setflags) {
        emit_code("      tmp = ~r%d + rhs;", Rn);
        emit_code("      APSR.N = !!(tmp & 0x80000000);");
        emit_code("      APSR.Z = (tmp == 0);");
        emit_code("      APSR.C = (tmp < rhs);");
        emit_code("      APSR.V = !((~r%d ^ rhs) & 0x80000000) && ((tmp ^ rhs) & 0x80000000));",
            Rn);
        emit_code("      r%d = tmp;", Rd);
    } else {
        emit_code("      r%d = ~r%d + rhs;", Rd, Rn);
    }

    emit_code("    }");

    pc_stack_push(pc + 4);
}

void
p_mul(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 16) & 0x0f;
    const uint32_t Rm = (code >> 8) & 0x0f;
    const uint32_t Rn = code & 0x0f;

    assert(Rd != 15);
    assert(Rm != 15);
    assert(Rn != 15);

    emit_code("    r%d = r%d * r%d;", Rd, Rn, Rm);
    if (setflags) {
        emit_code("    APSR.N = !!(r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        // C unchanged
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_rsb_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm12 = code & 0xfff;
    const uint32_t imm32 = arm_expand_imm12(imm12) + 1;

    assert(Rn != 15);
    assert(Rd != 15);

    if (setflags) {
        emit_code("    tmp = ~r%d + %u;", Rn, imm32);
        emit_code("    APSR.N = !!(tmp && 0x80000000);");
        emit_code("    APSR.Z = (tmp == 0);");
        emit_code("    APSR.C = (tmp < %u);", imm32);
        emit_code("    APSR.V = !((~r%d ^ %u) & 0x80000000) && ((tmp ^ %u) & 0x80000000);");
        emit_code("    r%d = tmp;", Rd);
    } else {
        emit_code("    r%d = ~r%d + %u;", Rd, Rn, imm32);
    }

    pc_stack_push(pc + 4);
}

void
p_lsr_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t Rm = code & 0x0f;
    const uint32_t shift_n = arm_decode_imm_shift(0b01, imm5);

    assert(Rd != 15);
    assert(Rm != 15);

    if (setflags && (shift_n > 0))
        emit_code("    APSR.C = (r%d >> %d) & 1;", Rm, shift_n - 1);

    emit_code("    r%d = r%d >> %d;", Rd, Rm, shift_n);
    if (setflags) {
        emit_code("    APSR.N = !!(r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_bx(uint32_t pc, uint32_t code)
{
    (void)pc;
    const uint32_t Rm = code & 0x0f;

    assert(Rm == 14);   // only `bx lr' supported now

    set_function_end_flag();
    emit_code("    return;");
}

void
p_ldrh_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = !!(code & (1 << 24));
    const uint32_t add = !!(code & (1 << 23));
    const uint32_t wback = !index && (code & (1 << 21));

    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rt = (code >> 12) & 0x0f;
    const uint32_t imm32 = code & 0xfff;
    const int32_t offset = add ? imm32 : -imm32;

    assert(Rt != 15);

    if (15 == Rn) {
        if (wback)
            assert(0 && "writeback in ldrh with Rn = pc");
        emit_code("   r%d = %u;", Rt, 0xffff & get_word_at(pc + 8 + (index ? offset : 0)));
    } else {
        if (index && !wback) {
            emit_code("   r%d = load_halfword(r%d + %d);", Rt, Rn, offset);
        } else if (index && wback) {
            emit_code("   r%d = r%d + %d;", Rn, Rn, offset);
            emit_code("   r%d = load_halfword(r%d);", Rt, Rn);
        } else if (!index) {
            emit_code("   r%d = load_halfword(r%d);", Rt, Rn);
            if (wback)
                emit_code("   r%d = r%d + %d;", Rn, Rn, offset);
        }
    }

    pc_stack_push(pc + 4);
}

void
p_str_register(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rt = (code >> 12) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x0f;
    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t Rm = code & 0x0f;
    const enum SRType shift_t = arm_decode_imm_type(type, imm5);
    const uint32_t shift_n = arm_decode_imm_shift(type, imm5);
    const char add_op = add ? '+' : '-';

    assert(Rn != 15);
    assert(Rt != 15);
    assert(Rm != 15);

    if (index && !wback) {
        switch (shift_t) {
        case SRType_LSL:
            emit_code("    store(r%d %c (r%d << %u), r%d);", Rn, add_op, Rm, shift_n, Rt);
            break;
        case SRType_LSR:
            emit_code("    store(r%d %c (r%d >> %u), r%d);", Rn, add_op, Rm, shift_n, Rt);
            break;
        default:
            assert(0 && "not implemented shift operation");
        }
    } else if (index && wback) {
        switch (shift_t) {
        case SRType_LSL:
            emit_code("    r%d = r%d %c (r%d << %u);", Rn, Rn, add_op, Rm, shift_n);
            break;
        case SRType_LSR:
            emit_code("    r%d = r%d %c (r%d >> %u);", Rn, Rn, add_op, Rm, shift_n);
            break;
        default:
            assert(0 && "not implemented shift operation");
        }

        emit_code("    store(r%d, r%d);", Rn, Rt);
    } else if (!index) {
        emit_code("    store(r%d, r%d);", Rn, Rt);
        if (wback) {
            switch (shift_t) {
            case SRType_LSL:
                emit_code("    r%d = r%d %c (r%d << %u);", Rn, Rn, add_op, Rm, shift_n);
                break;
            case SRType_LSR:
                emit_code("    r%d = r%d %c (r%d >> %u);", Rn, Rn, add_op, Rm, shift_n);
                break;
            default:
                assert(0 && "not implemented shift operation");
            }
        }
    }

    pc_stack_push(pc + 4);
}

void
p_ldr_register(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rt = (code >> 12) & 0x0f;
    const uint32_t imm5 = (code >> 7) & 0x0f;
    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t Rm = code & 0x0f;
    const enum SRType shift_t = arm_decode_imm_type(type, imm5);
    const uint32_t shift_n = arm_decode_imm_shift(type, imm5);
    const char add_op = add ? '+' : '-';

    assert(Rn != 15);
    assert(Rt != 15);
    assert(Rm != 15);

    if (index && !wback) {
        switch (shift_t) {
        case SRType_LSL:
            emit_code("    r%d = load(r%d %c (r%d << %u));", Rt, Rn, add_op, Rm, shift_n);
            break;
        case SRType_LSR:
            emit_code("    r%d = load(r%d %c (r%d >> %u));", Rt, Rn, add_op, Rm, shift_n);
            break;
        default:
            assert(0 && "not implemented shift operation");
        }
    } else if (index && wback) {
        switch (shift_t) {
        case SRType_LSL:
            emit_code("    r%d = r%d %c (r%d << %u);", Rn, Rn, add_op, Rm, shift_n);
            break;
        case SRType_LSR:
            emit_code("    r%d = r%d %c (r%d >> %u);", Rn, Rn, add_op, Rm, shift_n);
            break;
        default:
            assert(0 && "not implemented shift operation");
        }
        emit_code("    r%d = load(r%d);", Rt, Rn);
    } else if (!index) {
        emit_code("    r%d = load(r%d);", Rt, Rn);
        if (wback) {
            switch (shift_t) {
            case SRType_LSL:
                emit_code("    r%d = r%d %c (r%d << %u);", Rn, Rn, add_op, Rm, shift_n);
                break;
            case SRType_LSR:
                emit_code("    r%d = r%d %c (r%d >> %u);", Rn, Rn, add_op, Rm, shift_n);
                break;
            default:
                assert(0 && "not implemented shift operation");
            }
        }
    }

    pc_stack_push(pc + 4);
}

void
p_bic_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rn = (code >> 16) & 0x0f;
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t imm12 = code & 0xfff;
    const uint32_t imm32 = arm_expand_imm12(imm12);

    emit_code("    r%d = r%d & %uu;", Rd, Rn, ~imm32);
    if (setflags) {
        emit_code("    APSR.N = !!(r%d & 0x80000000);", Rd);
        emit_code("    APSR.Z = (r%d == 0);", Rd);
        if (imm12 & 0xf00)  // change C only if imm12 includes rotation
            emit_code("    APSR.C = %d;", !!(imm32 & 0x80000000));
        // V unchanged
    }

    pc_stack_push(pc + 4);
}

void
p_ldrb_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rn = (code >> 16) & 0xf;
    const uint32_t Rt = (code >> 12) & 0xf;
    const uint32_t imm32 = code & 0xfff;
    const int32_t offset = add ? imm32 : -imm32;

    assert(Rt != 15);

    if (15 == Rn) {
        assert(0 && "ldrb with Rn == 15 not implemented yet");
    } else {
        if (index && !wback) {
            emit_code("    r%u = load_byte(r%u + %d);", Rt, Rn);
        } else if (index && wback) {
            emit_code("    r%u += %d;", Rn, offset);
            emit_code("    r%u = load_byte(r%u);", Rt, Rn);
        } else if (!index && wback) {
            emit_code("    r%u = load_byte(r%u);", Rt, Rn);
            emit_code("    r%u += %d;", Rn, offset);
        }
    }

    pc_stack_push(pc + 4);
}

void
p_strb_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rn = (code >> 16) & 0xf;
    const uint32_t Rt = (code >> 12) & 0xf;
    const uint32_t imm32 = code & 0xfff;
    const int32_t offset = add ? imm32 : -imm32;

    assert(Rt != 15);
    if (15 == Rn) {
        assert(0 && "strb with Rn == 15 not implemented");
    } else {
        if (index && !wback) {
            emit_code("    store_byte(r%u + %d, r%u);",  Rn, Rt);
        } else if (index && wback) {
            emit_code("    r%u += %d;", Rn, offset);
            emit_code("    store_byte(r%u, r%u);", Rn, Rt);
        } else if (!index && wback) {
            emit_code("    store_byte(r%u, r%u);", Rn, Rt);
            emit_code("    r%u += %d;", Rn, offset);
        }
    }

    pc_stack_push(pc + 4);
}

void
p_ldrsb_immediate(uint32_t pc, uint32_t code)
{
    const uint32_t index = code & (1 << 24);
    const uint32_t add = code & (1 << 23);
    const uint32_t wback = !index || (code & (1 << 21));
    const uint32_t Rn = (code >> 16) & 0xf;
    const uint32_t Rt = (code >> 12) & 0xf;
    const uint32_t imm4H = (code >> 8) & 0xf;
    const uint32_t imm4L = code & 0xf;
    const uint32_t imm32 = (imm4H << 4) | imm4L;
    const uint32_t offset = add ? imm32 : -imm32;

    assert(Rt != 15);

    if (15 == Rn) {
        assert(0 && "ldrsb with Rn == 15 not implemented yet");
    } else {
        if (index && !wback) {
            emit_code("    r%u = load_byte(r%u + %d);", Rt, Rn);
        } else if (index && wback) {
            emit_code("    r%u += %d;", Rn, offset);
            emit_code("    r%u = load_byte(r%u);", Rt, Rn);
        } else if (!index && wback) {
            emit_code("    r%u = load_byte(r%u);", Rt, Rn);
            emit_code("    r%u += %d;", Rn, offset);
        }
        emit_code("    if (r%u & 0x80) r%u |= 0xffffff00;", Rt, Rt);
    }

    pc_stack_push(pc + 4);
}

void
p_blx_register(uint32_t pc, uint32_t code)
{
    const uint32_t Rm = code & 0xf;

    emit_code("    find_and_call_function(r%u);", Rm);
    pc_stack_push(pc + 4);
}

void
p_umull(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1<<20);
    const uint32_t RdHi = (code >> 16) & 0xf;
    const uint32_t RdLo = (code >> 12) & 0xf;
    const uint32_t Rm = (code >> 8) & 0xf;
    const uint32_t Rn = code & 0xf;

    emit_code("    {");
    emit_code("      uint64_t tmp = r%u * r%u;", Rn, Rm);
    emit_code("      r%u = tmp & 0xffffffff;", RdLo);
    emit_code("      r%u = tmp >> 32;", RdHi);
    if (setflags) {
        emit_code("      APSR.N = !!(r%u & 0x80000000);", RdHi);
        emit_code("      APSR.Z = (0 == r%u && 0 == r%u);", RdLo, RdHi);
        // C and V unchanged
    }
    emit_code("    }");

    pc_stack_push(pc + 4);
}

void
p_mla(uint32_t pc, uint32_t code)
{
    const uint32_t setflags = code & (1 << 20);
    const uint32_t Rd = (code >> 16) & 0xf;
    const uint32_t Ra = (code >> 12) & 0xf;
    const uint32_t Rm = (code >> 8) & 0xf;
    const uint32_t Rn = code & 0xf;

    emit_code("    r%u = r%u * r%u + r%u;", Rd, Rn, Rm, Ra);
    if (setflags) {
        emit_code("    APSR.N = !!(r%u & 0x80000000);", Rd);
        emit_code("    APSR.Z = (0 == r%u);", Rd);
        // V and C unchanged
    }

    pc_stack_push(pc + 4);
}

void
process_instruction(uint32_t pc)
{
    uint32_t code = get_word_at(pc);
    flag_last_instruction_ended_with_return = 0;

    emit_code("label_%04x:", pc);

    const uint32_t condition = (code >> 28) & 0x0f;
    switch (condition) {
    case 0b0000: emit_code("  if (APSR.Z) {"); break;
    case 0b0001: emit_code("  if (!APSR.Z) {"); break;
    case 0b0010: emit_code("  if (APSR.C) {"); break;
    case 0b0011: emit_code("  if (!APSR.C) {"); break;
    case 0b0100: emit_code("  if (APSR.N) {"); break;
    case 0b0101: emit_code("  if (!APSR.N) {"); break;
    case 0b0110: emit_code("  if (APSR.V) {"); break;
    case 0b0111: emit_code("  if (!APSR.V) {"); break;
    case 0b1000: emit_code("  if (APSR.C && !APSR.Z) {"); break;
    case 0b1001: emit_code("  if (!APSR.C || APSR.Z) {"); break;
    case 0b1010: emit_code("  if (!!APSR.N == !!APSR.V) {"); break;
    case 0b1011: emit_code("  if (!!APSR.N != !!APSR.V) {"); break;
    case 0b1100: emit_code("  if (!APSR.Z && !!APSR.N == !!APSR.V) {"); break;
    case 0b1101: emit_code("  if (APSR.Z || !!APSR.N != !!APSR.V) {"); break;
    case 0b1110: /* unconditional, no code emited */ break;
    default: assert(0 && "wierd condition"); break;
    }


    if ((code & 0x0fd00000) == 0x09000000) {
        p_stmdb(pc, code);
    } else if ((code & 0x0fe00000) == 0x02400000) {
        p_sub_immediate(pc, code);
    } else if ((code & 0x0fe00090) == 0x00400010) {
        p_sub_register_shifted_register(pc, code);
    } else if ((code & 0x0e500000) == 0x04000000) {
        p_str_immediate(pc, code);
    } else if ((code & 0x0e500000) == 0x04100000) {
        p_ldr_immediate(pc, code);
    } else if ((code & 0x0e5000f0) == 0x004000d0) {
        p_ldrd_immediate(pc, code);
    } else if ((code & 0x0fe00010) == 0x00800000) { // add register
        p_addsubcarry_register(pc, code, /* do_add = */ 1, /* do_carry = */ 0);
    } else if ((code & 0x0fe00010) == 0x00a00000) { // adc register
        p_addsubcarry_register(pc, code, /* do_add = */ 1, /* do_carry = */ 1);
    } else if ((code & 0x0fe00010) == 0x00400000) { // sub register
        p_addsubcarry_register(pc, code, /* do_add = */ 0, /* do_carry = */ 0);
    } else if ((code & 0x0fe00010) == 0x00c00000) { // sbc register
        p_addsubcarry_register(pc, code, /* do_add = */ 0, /* do_carry = */ 1);
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
    } else if ((code & 0x0fd00000) == 0x08900000) {
        p_ldm(pc, code);
    } else if ((code & 0x0e5000f0) == 0x004000f0) {
        p_strd_immediate(pc, code);
    } else if ((code & 0x0ff00010) == 0x01500000) {
        p_cmp_register(pc, code);
    } else if ((code & 0x0fe00070) == 0x01a00000) {
        p_lsl_immediate(pc, code);
    } else if ((code & 0x0fe00000) == 0x02000000) {
        p_and_immediate(pc, code);
    } else if ((code & 0x0fe00070) == 0x01a00040) {
        p_asr_immediate(pc, code);
    } else if ((code & 0x0e5000f0) == 0x004000b0) {
        p_strh_immediate(pc, code);
    } else if ((code & 0x0fe00010) == 0x01800000) {
        p_orr_register(pc, code);
    } else if ((code & 0x0fe00000) == 0x03e00000) {
        p_mvn_immediate(pc, code);
    } else if ((code & 0x0fe00010) == 0x00600000) {
        p_rsb_register(pc, code);
    } else if ((code & 0x0fe000f0) == 0x00000090) {
        p_mul(pc, code);
    } else if ((code & 0x0fe00000) == 0x02600000) {
        p_rsb_immediate(pc, code);
    } else if ((code & 0x0fe00070) == 0x01a00020) {
        p_lsr_immediate(pc, code);
    } else if ((code & 0x0e5000f0) == 0x005000b0) {
        p_ldrh_immediate(pc, code);
    } else if ((code & 0x0ff000f0) == 0x01200010) {
        p_bx(pc, code);
    } else if ((code & 0x0e500010) == 0x06000000) {
        p_str_register(pc, code);
    } else if ((code & 0x0e500010) == 0x06100000) {
        p_ldr_register(pc, code);
    } else if ((code & 0x0fe00000) == 0x03c00000) {
        p_bic_immediate(pc, code);
    } else if ((code & 0x0e500000) == 0x04500000) {
        p_ldrb_immediate(pc, code);
    } else if ((code & 0x0e500000) == 0x04400000) {
        p_strb_immediate(pc, code);
    } else if ((code & 0x0e5000f0) == 0x005000d0) {
        p_ldrsb_immediate(pc, code);
    } else if ((code & 0x0ffffff0) == 0x012fff30) {
        p_blx_register(pc, code);
    } else if ((code & 0x0fe000f0) == 0x00800090) {
        p_umull(pc, code);
    } else if ((code & 0x0fe000f0) == 0x00200090) {
        p_mla(pc, code);
    } else {
        printf("process_instruction(0x%04x, 0x%08x)\n", pc, code);
        assert(0 && "instruction code not implemented");
    }

    if (condition != 0x0e)
        emit_code("  }");
}
