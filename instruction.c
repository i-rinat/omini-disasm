#include "instruction.h"
#include "output.h"
#include <stdio.h>
#include <assert.h>

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
