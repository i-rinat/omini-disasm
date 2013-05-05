#include "instruction.h"
#include "output.h"
#include <stdio.h>
#include <assert.h>

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


void
p_push(uint32_t code)
{
    const uint32_t register_list = code & 0xffff;
    uint32_t mask = 1;
    for (int reg = 0; reg < 16; reg ++) {
        if (register_list & mask)
            emit_output("   push(%d);", reg);
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

    emit_output("   r%d = r%d - %d;", Rd, Rn, imm32);
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
    const uint32_t imm5 = (code >> 7) & 0x1f;
    const uint32_t type = (code >> 5) & 0x03;
    const uint32_t Rd = (code >> 12) & 0x0f;
    const uint32_t Rn = (code >> 16) & 0x0f;

    assert(Rd != 15);
    assert(Rd != 14);

    int setflags = (code >> 20) & 1;
    if (setflags) {
        assert(0);
    }
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
        emit_output("   store(r%d + %d, r%d);", Rn, imm32, Rt);
    } else if (index && wback) {
        emit_output("   r%d = r%d + %d;", Rn, Rn, imm32);
        emit_output("   store(r%d, r%d);", Rn, Rt);
    } else if (!index && wback) {
        emit_output("   store(r%d, r%d);", Rn, Rt);
        emit_output("   r%d = r%d + %d;", Rn, Rn, imm32);
    } else if (!index && !wback) {
        emit_output("   store(r%d, r%d);", Rn, Rt);
    } else {
        assert(0 && "logic error");
    }
}


void
process_instruction(uint32_t code)
{
    printf("process_instruction(0x%x)\n", code);

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
    } else {
        assert(0 && "instruction code not implemented");
    }
}
