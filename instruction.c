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
p_push(uint32_t register_list)
{
    uint32_t mask = 1;
    for (int reg = 0; reg < 16; reg ++) {
        if (register_list & mask)
            emit_output("   push(%d);", reg);
        mask <<= 1;
    }
}




void
process_instruction(uint32_t code)
{
    printf("process_instruction(0x%x)\n", code);

    if ((code & 0x0fff0fff) == 0x052d0004) {
        printf("push (single)\n");
        assert(0 && "not implemented");
    } else
    if ((code & 0x0fff0000) == 0x092d0000) {
        p_push(code & 0xffff);
    } else
    {
        assert(0 && "not implemented");
    }
}
