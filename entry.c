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
    visited_bitmap_initialize();
    pc_stack_push(start_pc);
    begin_function(start_pc);
    uint32_t predicted_pc = start_pc;
    while (! pc_stack_empty()) {
        uint32_t pc = pc_stack_pop();
        if (predicted_pc != pc) {
            if (visited_bitmap_visited(predicted_pc))
                emit_code("    goto label_%04x;", predicted_pc);
        }
        if (visited_bitmap_visited(pc))
            continue;
        process_instruction(pc);
        visited_bitmap_mark_visited(pc);
        predicted_pc = pc + 4;
    }

    assert(function_end_found());
    end_function();
    visited_bitmap_free();
    pc_stack_free();
}

int
main(void)
{
    printf("rec started\n");

    FILE *fp = fopen("libplasma.so", "rb");
    uint32_t *text = read_section(fp, 0x12e8, 0x2df0);
    fclose(fp);

    set_output_file("code.c");

    func_list_initialize();
    func_list_add(0x2170);

    while (func_list_get_count() > 0) {
        printf("functions in list: %u (%u)\n", func_list_get_count(), func_list_get_done_count());
        uint32_t func_pc = func_list_get_next();
        printf("processing function 0x%04x\n", func_pc);
        process_function(func_pc);
        func_list_mark_done(func_pc);
    }

    func_list_free();
    close_output_file();

    free(text);
    printf("done\n");
    return 0;
}
