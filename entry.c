#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "section.h"
#include "pc-stack.h"
#include "output.h"
#include <assert.h>

void
process_function(uint32_t start_pc)
{
    pc_stack_initialize();
    pc_stack_push(start_pc);
    begin_function(start_pc);
    uint32_t next_pc = start_pc;
    while (! pc_stack_empty()) {
        uint32_t pc = pc_stack_pop();
        process_instruction(pc);
        next_pc = pc + 4;
        (void)next_pc;
    }

    assert(function_end_found());
    end_function();
    pc_stack_free();
}

int
main(void)
{
    printf("rec started\n");

    FILE *fp = fopen("libplasma.so", "rb");
    uint32_t *text = read_section(fp, 0x12e8, 0x2df0);
    fclose(fp);

    //~ for (int k = 0; k < 90; k ++)
        //~ process_instruction(0x2170 + k * 4);

    set_output_file("code.c");
    process_function(0x2170);
    close_output_file();

    free(text);
    return 0;
}
