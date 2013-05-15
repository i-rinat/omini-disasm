#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "section.h"
#include "pc-stack.h"
#include "output.h"
#include <assert.h>
#include <bfd.h>

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

void
whatever(const char *fname)
{
    bfd *obj = bfd_openr("libplasma.so", NULL);
    if (NULL == obj) {
        printf("bfd_openr failed\n");
        return;
    }

    if (!bfd_check_format(obj, bfd_object)) {
        if (bfd_get_error() != bfd_error_file_ambiguously_recognized) {
            printf("bfd not recognized binary\n");
            return;
        }
    }

    printf("section list:\n");
    for (asection *s = obj->sections; s; s = s->next) {
        if (SEC_LOAD & bfd_get_section_flags(obj, s)) {
            if (bfd_section_lma(obj, s) != bfd_section_vma(obj, s)) {
                assert(0);
            } else {
                printf("   loadable section: %s, addr = 0x%04x, size = 0x%04x\n",
                    bfd_section_name(obj, s), bfd_section_lma(obj, s), bfd_section_size(obj, s));
            }
        } else {
            printf("   non-loadable section: %s, addr = 0x%04x, size = 0x%04x\n",
                bfd_section_name(obj, s), bfd_section_lma(obj, s), bfd_section_size(obj, s));
        }
    }

}

int
main(void)
{
    printf("rec started\n");

    whatever("libplasma.so");

    FILE *fp = fopen("libplasma.so", "rb");
    uint32_t *text = read_section(fp, 0x12e8, 0x2df0);
    fclose(fp);

    set_output_file("code.c");

    emit_code("#include <stdint.h>");
    emit_code("#include \"registers.inc\"");
    emit_code("#include \"prototypes.inc\"");
    emit_code("");

    func_list_initialize();
    func_list_add(0x2170);

    func_list_add_to_done_list(0x2304);
    func_list_add_to_done_list(0x230c);
    func_list_add_to_done_list(0x2310);
    func_list_add_to_done_list(0x25bc);
    func_list_add_to_done_list(0x25e0);
    func_list_add_to_done_list(0x2608);
    func_list_add_to_done_list(0x2648);
    func_list_add_to_done_list(0x265c);
    func_list_add_to_done_list(0x26bc);
    func_list_add_to_done_list(0x2928);
    func_list_add_to_done_list(0x2b2c);
    func_list_add_to_done_list(0x2b34);
    func_list_add_to_done_list(0x2b3c);
    func_list_add_to_done_list(0x2bc4);
    func_list_add_to_done_list(0x2be0);
    func_list_add_to_done_list(0x2bf4);
    func_list_add_to_done_list(0x2c08);
    func_list_add_to_done_list(0x2c1c);
    func_list_add_to_done_list(0x2c30);
    func_list_add_to_done_list(0x2c44);
    func_list_add_to_done_list(0x2c58);
    func_list_add_to_done_list(0x3078);
    func_list_add_to_done_list(0x30e4);
    func_list_add_to_done_list(0x34e8);
    func_list_add_to_done_list(0x34f0);
    func_list_add_to_done_list(0x34f8);
    func_list_add_to_done_list(0x3500);
    func_list_add_to_done_list(0x3858);
    func_list_add_to_done_list(0x3860);
    func_list_add_to_done_list(0x3904);
    func_list_add_to_done_list(0x3920);
    func_list_add_to_done_list(0x398c);
    func_list_add_to_done_list(0x39ac);
    func_list_add_to_done_list(0x39b0);
    func_list_add_to_done_list(0x39d0);
    func_list_add_to_done_list(0x3a90);
    func_list_add_to_done_list(0x3aa4);
    func_list_add_to_done_list(0x3aac);
    func_list_add_to_done_list(0x3ab4);
    func_list_add_to_done_list(0x3abc);
    func_list_add_to_done_list(0x3ac4);
    func_list_add_to_done_list(0x3acc);
    func_list_add_to_done_list(0x3ad4);
    func_list_add_to_done_list(0x3b18);
    func_list_add_to_done_list(0x3b5c);
    func_list_add_to_done_list(0x3b70);
    func_list_add_to_done_list(0x3b84);
    func_list_add_to_done_list(0x3ba8);
    func_list_add_to_done_list(0x3bcc);
    func_list_add_to_done_list(0x3bf0);
    func_list_add_to_done_list(0x3c14);
    func_list_add_to_done_list(0x3c90);
    func_list_add_to_done_list(0x4024);
    func_list_add_to_done_list(0x4064);
    func_list_add_to_done_list(0x4090);
    func_list_add_to_done_list(0x40c8);
    func_list_add_to_done_list(0x40d0);

    while (func_list_get_count() > 0) {
        printf("functions in list: %u (%u)\n", func_list_get_count(), func_list_get_done_count());
        uint32_t func_pc = func_list_get_next();
        printf("processing function 0x%04x\n", func_pc);
        if (func_pc >= 0x12e8)
            process_function(func_pc);
        func_list_mark_done(func_pc);
    }

    close_output_file();

    // generate prototypes

    set_output_file("prototypes.inc");
    uint32_t func_pc;
    while (0 != (func_pc = func_list_pop_from_done_list())) {
        emit_code("void func_%04x();", func_pc);
    }

    close_output_file();

    func_list_free();

    free(text);
    printf("done\n");
    return 0;
}
