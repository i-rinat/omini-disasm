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
        const char *symname = symbol_table[k]->name;
        const char *sectname = symbol_table[k]->section->name;
        uint32_t func_addr = symbol_table[k]->value + symbol_table[k]->section->vma;

        printf("sym name: %s, of section %s, value = %x\n", symname, sectname, func_addr);
        if (func_list_already_in_done_list(func_addr))
            continue;

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
            } else if (!strcmp(symname, "__gnu_unwind_execute")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_dcmpeq")) {
                emit_code("void func_%04x() { r0 = reg.x_double == reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmplt")) {
                emit_code("void func_%04x() { r0 = reg.x_double < reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmple")) {
                emit_code("void func_%04x() { r0 = reg.x_double <= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmpge")) {
                emit_code("void func_%04x() { r0 = reg.x_double >= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmpgt")) {
                emit_code("void func_%04x() { r0 = reg.x_double > reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__cmpdf2")) {
                // this is some underlying stuff, skip it
            } else if (!strcmp(symname, "__eqdf2")) {
                // alias for __cmpdf2
            } else if (!strcmp(symname, "__floatdidf")) {
                // alias for __aeabi_l2d
            } else if (!strcmp(symname, "__aeabi_l2d")) {
                emit_code("void func_%04x() { reg.x_double = reg.x_uint64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_unwind_cpp_pr0")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_unwind_cpp_pr1")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_unwind_cpp_pr2")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_unwind_frame")) {
                // do nothing
            } else if (!strcmp(symname, "__gtdf2")) {
                // frontend for __cmpdf2
            } else if (!strcmp(symname, "__ledf2")) {
                // frontend for __cmpdf2
            } else if (!strcmp(symname, "__ltdf2")) {
                // frontend for __cmpdf2
            } else if (!strcmp(symname, "__aeabi_cdcmple")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_cdcmpeq")) {
                // do nothing
            } else if (!strcmp(symname, "__restore_core_regs")) {
                // do nothing
            } else if (!strcmp(symname, "restore_core_regs")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_cdrcmple")) {
                // do nothing
            } else if (!strcmp(symname, "__floatsidf")) {
                // alias for __aeabi_i2d
            } else if (!strcmp(symname, "__aeabi_i2d")) {
                emit_code("void func_%04x() { reg.x_double = reg.r0_signed; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ui2d")) {
                emit_code("void func_%04x() { reg.x_double = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dsub")) {
                emit_code(
                    "void func_%04x() { reg.x_double = reg.x_double - reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_drsub")) {
                emit_code(
                    "void func_%04x() { reg.x_double = reg.y_double - reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__extendsfdf2")) {
                emit_code("void func_%04x() { reg.x_double = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ddiv")) {
                emit_code("void func_%04x() { reg.x_double /= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__divdf3")) {
                emit_code("void func_%04x() { reg.x_double /= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__adddf3")) {
                emit_code("void func_%04x(){ reg.x_double += reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__subdf3")) {
                emit_code("void func_%04x(){ reg.x_double -= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dmul")) {
                emit_code("void func_%04x(){ reg.x_double *= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__muldf3")) {
                emit_code("void func_%04x(){ reg.x_double *= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dadd")) {
                emit_code("void func_%04x(){ reg.x_double += reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatunsidf")) {
                emit_code("void func_%04x(){ reg.x_double = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__fixdfsi")) {
                emit_code("void func_%04x(){ reg.r0_signed = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ul2d")) {
                emit_code("void func_%04x(){ reg.x_double = reg.x_uint64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatundidf")) {
                emit_code("void func_%04x(){ reg.x_double = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__nedf2")) {
                emit_code("void func_%04x(){ r0 = (reg.x_double != reg.y_double); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__gedf2")) {
                emit_code("void func_%04x(){ r0 = (reg.x_double >= reg.y_double); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_d2iz")) {
                emit_code("void func_%04x(){ reg.r0_signed = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_f2d")) {
                emit_code("void func_%04x(){ reg.x_double = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__gnu_uldivmod_helper")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_ldivmod_helper")) {
                // do nothing
            } else if (!strcmp(symname, "__divdi3")) {
                // do nothing. Let recompler do its work
            } else if (!strcmp(symname, "__divsf3")) {
                emit_code("void func_%04x(){ reg.x_float /= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__fixsfsi")) {
                emit_code("void func_%04x(){ reg.x_signed = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fadd")) {
                emit_code("void func_%04x(){ reg.x_float += reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_frsub")) {
                emit_code("void func_%04x(){ reg.x_float = reg.y_float - reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__subsf3")) {
                emit_code("void func_%04x(){ reg.x_float -= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__mulsf3")) {
                emit_code("void func_%04x(){ reg.x_float *= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpeq")) {
                emit_code("void func_%04x() { r0 = reg.x_float == reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmplt")) {
                emit_code("void func_%04x() { r0 = reg.x_float < reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmple")) {
                emit_code("void func_%04x() { r0 = reg.x_float <= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpge")) {
                emit_code("void func_%04x() { r0 = reg.x_float >= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpgt")) {
                emit_code("void func_%04x() { r0 = reg.x_float > reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_uldivmod")) {
                // do nothing
            } else if (!strcmp(symname, "__udivsi3")) {
                emit_code("void func_%04x(){ r0 = r0 / r1; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_i2f")) {
                emit_code("void func_%04x(){ reg.x_float = reg.r0_signed; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__lesf2")) {
                emit_code("void func_%04x(){ r0 = (reg.x_float <= reg.y_float); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__gtsf2")) {
                emit_code("void func_%04x(){ r0 = (reg.x_float > reg.y_float); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__nesf2")) {
                emit_code("void func_%04x(){ r0 = (reg.x_float != reg.y_float); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__fixunsdfsi")) {
                emit_code("void func_%04x(){ reg.r0_signed = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__udivdi3")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_d2f")) {
                emit_code("void func_%04x(){ reg.x_float = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatdisf")) {
                emit_code("void func_%04x(){ reg.x_float = reg.x_int64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_idiv")) {
                emit_code("void func_%04x(){ reg.r0_signed /= reg.r1_signed; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatundisf")) {
                emit_code("void func_%04x(){ reg.x_float = reg.x_uint64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ui2f")) {
                emit_code("void func_%04x(){ reg.x_float = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__div0")) {
                // please don't divide by zero
            } else if (!strcmp(symname, "__aeabi_cfcmple")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_cfrcmple")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_cfcmpeq")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_uidivmod")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_idivmod")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_ldivmod")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_f2uiz")) {
                emit_code("void func_%04x(){ r0 = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else if (!strncmp(symname, "Java_", 5) || !strncmp(symname, "JNI_", 4)) {
                // JNI entry
                func_list_add(func_addr);
            } else if (!strncmp(symname, "java", 4)) {
                // maybe JNI entry
                func_list_add(func_addr);
            } else if (!strcmp(symname, "passed_paint_time_limit")) { // TODO: libom.so specific
                func_list_add(func_addr);
            } else {
                printf("unknown symbol %s\n", symname);
                assert(0 && "not implemented");
            }
        } else if (!strcmp(sectname, ".data")) {
            if (!strcmp(symname, ".data")) {
                // do nothing
            } else if (!strcmp(symname, "__data_start")) {
                // TODO: what to do?
            } else {
                printf("unknown symbol %s\n", symname);
                assert(0 && "not implemented");
            }
        } else if (!strcmp(sectname, "*ABS*")) {
            printf("stub for %s\n", sectname);
            // TODO: do something
        } else if (!strcmp(sectname, "*UND*")) {
            printf("stub for %s\n", sectname);
            // TODO: do something
        } else if (!strcmp(sectname, ".fini_array")) {
            printf("stub for %s\n", sectname);
            // TODO: do something
        } else if (!strcmp(sectname, ".init_array")) {
            printf("stub for %s\n", sectname);
            // TODO: do something
        } else if (!strcmp(sectname, ".data.rel.ro")) {
            printf("stub for %s\n", sectname);
            // TODO: do something
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

    read_section(abfd, ".text");

    determine_target_functions(abfd);

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

    printf("done\n");
    return 0;
}
