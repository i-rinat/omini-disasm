#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "section.h"
#include "pc-stack.h"
#include "output.h"
#include <assert.h>
#include <bfd.h>
#include <string.h>

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

static
void
determine_target_functions(bfd *abfd)
{

    int storage_needed = bfd_get_dynamic_symtab_upper_bound(abfd);
    if (storage_needed <= 0)    // either error or empty symbol table
        return;

    asymbol **symbol_table = malloc(storage_needed);
    assert(symbol_table);

    int number_of_symbols = bfd_canonicalize_dynamic_symtab(abfd, symbol_table);
    printf("number_of_symbols = %d\n", number_of_symbols);

    assert(number_of_symbols > 0);

    for (int k = 0; k < number_of_symbols; k ++) {
        printf("sym name: %s, of section %s, value = %x\n", symbol_table[k]->name,
            symbol_table[k]->section->name,
            (int)(symbol_table[k]->value + symbol_table[k]->section->vma));

        const char *symname = symbol_table[k]->name;
        const char *sectname = symbol_table[k]->section->name;

        if (!strcmp(sectname, ".text")) {
            if (!strcmp(symname, ".text")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Backtrace")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_ForcedUnwind")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_RaiseException")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_VFP_D_16_to_31")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_VFP_D")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_VFP")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_WMMXC")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_WMMXD")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Resume")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Resume_or_Rethrow")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Save_VFP_D_16_to_31")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Save_VFP_D")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Save_VFP")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Save_WMMXC")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Save_WMMXD")) {
                // do nothing
            } else if (!strcmp(symname, "___Unwind_Backtrace")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_Backtrace")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_Complete")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_DeleteException")) {
                // do nothing
            } else if (!strcmp(symname, "___Unwind_ForcedUnwind")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_ForcedUnwind")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_GetCFA")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_GetDataRelBase")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_GetLanguageSpecificData")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_GetRegionStart")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_GetTextRelBase")) {
                // do nothing
            } else if (!strcmp(symname, "___Unwind_RaiseException")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_RaiseException")) {
                // do nothing
            } else if (!strcmp(symname, "___Unwind_Resume")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_Resume")) {
                // do nothing
            } else if (!strcmp(symname, "___Unwind_Resume_or_Rethrow")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_Resume_or_Rethrow")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_VRS_Get")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_VRS_Pop")) {
                // do nothing
            } else if (!strcmp(symname, "_Unwind_VRS_Set")) {
                // do nothing
            } else {
                printf("unknown symbol %s\n", symname);
                assert(0 && "not implemented");
            }
        } else if (!strcmp(sectname, ".data")) {
            if (!strcmp(symname, ".data")) {
                // do nothing
            } else {
                printf("unknown symbol %s\n", symname);
                assert(0 && "not implemented");
            }
        } else {
            printf("don't know how to process symbol of section '%s'\n", sectname);
            assert(0 && "not implemented");
        }
    }

    free(symbol_table);
}


int
main(void)
{
    printf("rec started\n");

    FILE *fp = fopen("libplasma.so", "rb");
    uint32_t *text = read_section(fp, 0x12e8, 0x2df0);
    fclose(fp);

    set_output_file("code.c");

    emit_code("#include <stdint.h>");
    emit_code("#include \"registers.inc\"");
    emit_code("#include \"prototypes.inc\"");
    emit_code("");

    func_list_initialize();

    bfd *abfd = bfd_openr("libplasma.so", NULL);
    if (!bfd_check_format(abfd, bfd_object))
        if (bfd_get_error() != bfd_error_file_ambiguously_recognized)
            assert(0 && "bfd not recognized binary");

    determine_target_functions(abfd);

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
