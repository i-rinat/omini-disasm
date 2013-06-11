#include "patterns.h"
#include "instruction.h"
#include "section.h"
#include "output.h"
#include "pc-stack.h"
#include <assert.h>

/** @brief detect and handle switch-case pattern

    cmp     rX, #Y
    addls   pc, pc, rX, lsl #2              <--- pc points here when function called
    b       loc_?                   \
    b       loc_?                   |
    b       loc_?                   |   (Y + 2) times
    ...                             |
    b       loc_?                   /

    @return 1 if pattern recognized and handled, 0 otherwise
*/

int
pattern_switch_case(uint32_t pc)
{
    uint32_t code;
    code = get_word_at(pc - 4);
    if ((code & 0xfff0f000) != 0xe3500000)   // cmp rX, #Y
        return 0;
    const uint32_t regX = (code >> 16) & 0xf;
    const uint32_t paramY = arm_expand_imm12(code & 0xfff);

    code = get_word_at(pc);
    if ((code & 0xfffffff0) != 0x908ff100)  // addls pc, pc, rX, lsl #2
        return 0;
    if ((code & 0xf) != regX)               // rX must be the same as in cmp instruction
        return 0;

    emit_code("  }");
    emit_code("    switch (r%u) {", regX);
    for (uint32_t k = 0; k < paramY + 2; k ++) {
        code = get_word_at(pc + 4 + k * 4);

        if ((code & 0xff000000) != 0xea000000)  // b loc_?
            return 0;
        const uint32_t offset = arm_sign_extend_imm24(code & 0x00ffffff) << 2;
        const uint32_t instruction_pc = pc + 4 + k * 4;
        const uint32_t target_pc = instruction_pc + 8 + offset; // may wrap
        if (0 == k) {
            emit_code("    default:");
        } else {
            emit_code("    case %d:", k - 1);
        }
        emit_code("        goto label_%04x;", target_pc);
        pc_stack_push(target_pc);
    }
    emit_code("    }");
    emit_code("  {");

    return 1;
}
