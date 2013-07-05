#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "section.h"
#include "pc-stack.h"
#include "output.h"
#include <assert.h>
#include <bfd.h>
#include <string.h>
#include "target-specific.h"
#include "relocations.h"

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
    if (!last_instruction_ended_with_return() && !last_instruction_ended_with_goto())
        emit_code("    goto label_%04x;", predicted_pc);

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

        // printf("sym name: %s, of section %s, value = %x\n", symname, sectname, func_addr);
        if (func_list_already_in_done_list(func_addr))
            continue;

        if (!strcmp(sectname, ".text")) {
            if (!strcmp(symname, ".text")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_Unwind_Backtrace")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_ForcedUnwind")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_RaiseException")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_VFP_D_16_to_31")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_VFP_D")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_VFP")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_WMMXC")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Restore_WMMXD")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Resume")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Resume_or_Rethrow")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Save_VFP_D_16_to_31")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Save_VFP_D")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Save_VFP")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Save_WMMXC")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_Unwind_Save_WMMXD")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "___Unwind_Backtrace")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_Backtrace")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_Complete")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_DeleteException")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "___Unwind_ForcedUnwind")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_ForcedUnwind")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_GetCFA")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_GetDataRelBase")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_GetLanguageSpecificData")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_GetRegionStart")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_GetTextRelBase")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "___Unwind_RaiseException")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_RaiseException")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "___Unwind_Resume")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_Resume")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "___Unwind_Resume_or_Rethrow")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_Resume_or_Rethrow")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_VRS_Get")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_VRS_Pop")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_Unwind_VRS_Set")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_unwind_execute")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmpeq")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_double == y_double); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmplt")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_double < y_double); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmple")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_double <= y_double); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmpge")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_double >= y_double); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmpgt")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_double > y_double); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__cmpdf2")) {
                // this is some underlying stuff, skip it
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__eqdf2")) {
                // alias for __cmpdf2
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__floatdidf") || !strcmp(symname, "__aeabi_l2d")) {
                emit_code("static void func_%04x(state_t *state) { x_double = x_int64_t; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_unwind_cpp_pr0")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_unwind_cpp_pr1")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_unwind_cpp_pr2")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_unwind_frame")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__cmpsf2")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gtdf2")) {
                // frontend for __cmpdf2
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__ledf2")) {
                // frontend for __cmpdf2
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__ltdf2")) {
                // frontend for __cmpdf2
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_cdcmple")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_cdcmpeq")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__restore_core_regs")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "restore_core_regs")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_cdrcmple")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__floatsidf") || !strcmp(symname, "__aeabi_i2d")) {
                emit_code("static void func_%04x(state_t *state) { x_double = r0_signed; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ui2d")) {
                emit_code("static void func_%04x(state_t *state) { x_double = r0; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dsub")) {
                emit_code("static void func_%04x(state_t *state) { x_double = x_double - y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_drsub")) {
                emit_code("static void func_%04x(state_t *state) { x_double = y_double - x_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__extendsfdf2")) {
                emit_code("static void func_%04x(state_t *state) { x_double = x_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ddiv")) {
                emit_code("static void func_%04x(state_t *state) { x_double /= y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__divdf3")) {
                emit_code("static void func_%04x(state_t *state) { x_double /= y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__adddf3")) {
                emit_code("static void func_%04x(state_t *state){ x_double += y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__subdf3")) {
                emit_code("static void func_%04x(state_t *state){ x_double -= y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dmul")) {
                emit_code("static void func_%04x(state_t *state){ x_double *= y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__muldf3")) {
                emit_code("static void func_%04x(state_t *state){ x_double *= y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dadd")) {
                emit_code("static void func_%04x(state_t *state){ x_double += y_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__floatunsidf")) {
                emit_code("static void func_%04x(state_t *state){ x_double = r0; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__fixdfsi")) {
                emit_code("static void func_%04x(state_t *state){ r0_signed = x_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ul2d")) {
                emit_code("static void func_%04x(state_t *state){ x_double = x_uint64_t; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__floatundidf")) {
                emit_code("static void func_%04x(state_t *state){ x_double = x_uint64_t; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__nedf2")) {
                emit_code("static void func_%04x(state_t *state){ r0 = (x_double != y_double); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gedf2")) {
                emit_code("static void func_%04x(state_t *state){ r0 = (x_double >= y_double); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_d2iz")) {
                emit_code("static void func_%04x(state_t *state){ r0_signed = x_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_f2iz")) {
                emit_code("static void func_%04x(state_t *state){ r0_signed = x_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_f2d")) {
                emit_code("static void func_%04x(state_t *state){ x_double = x_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_uldivmod_helper")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gnu_ldivmod_helper")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__divdi3")) {
                emit_code("static void func_%04x(state_t *state){ x_int64_t /= y_int64_t; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__divsf3") || !strcmp(symname, "__aeabi_fdiv")) {
                emit_code("static void func_%04x(state_t *state){ x_float /= y_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__fixsfsi")) {
                emit_code("static void func_%04x(state_t *state){ r0_signed = x_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fadd")) {
                emit_code("static void func_%04x(state_t *state){ x_float += y_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_frsub")) {
                emit_code("static void func_%04x(state_t *state){ x_float = y_float - x_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__subsf3") || !strcmp(symname, "__aeabi_fsub")) {
                emit_code("static void func_%04x(state_t *state){ x_float -= y_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__mulsf3") || !strcmp(symname, "__aeabi_fmul")) {
                emit_code("static void func_%04x(state_t *state){ x_float *= y_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpeq")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_float == y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmplt")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_float < y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmple")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_float <= y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpge")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_float >= y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpgt")) {
                emit_code("static void func_%04x(state_t *state) { r0 = (x_float > y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_uldivmod")) {
                emit_code("static void func_%04x(state_t *state) {", func_addr);
                // TODO: division by zero handling
                emit_code("    if (0 == y_uint64_t) return;");
                emit_code("    const uint64_t q = x_uint64_t / y_uint64_t;");
                emit_code("    const uint64_t r = x_uint64_t % y_uint64_t;");
                emit_code("    x_uint64_t = q;");
                emit_code("    y_uint64_t = r;");
                emit_code("}");
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__udivsi3")) {
                emit_code("static void func_%04x(state_t *state){ r0 /= r1; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__divsi3")) {
                emit_code("static void func_%04x(state_t *state){ r0_signed /= r1_signed; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_i2f")) {
                emit_code("static void func_%04x(state_t *state){ x_float = r0_signed; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__ltsf2")) {
                emit_code("static void func_%04x(state_t *state){ r0 = (x_float < y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__lesf2")) {
                emit_code("static void func_%04x(state_t *state){ r0 = (x_float <= y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__gtsf2")) {
                emit_code("static void func_%04x(state_t *state){ r0 = (x_float > y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__nesf2")) {
                emit_code("static void func_%04x(state_t *state){ r0 = (x_float != y_float); }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__fixunsdfsi")) {
                emit_code("static void func_%04x(state_t *state){ r0 = x_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__udivdi3")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_d2f")) {
                emit_code("static void func_%04x(state_t *state){ x_float = x_double; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__floatdisf")) {
                emit_code("static void func_%04x(state_t *state){ x_float = x_int64_t; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_idiv")) {
                emit_code("static void func_%04x(state_t *state){ r0_signed /= r1_signed; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__floatundisf")) {
                emit_code("static void func_%04x(state_t *state){ x_float = x_uint64_t; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ui2f")) {
                emit_code("static void func_%04x(state_t *state){ x_float = r0; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__div0")) {
                // please don't divide by zero
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ldiv0")) {
                // please don't divide by zero
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_idiv0")) {
                // please don't divide by zero
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_cfcmple")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_cfrcmple")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_cfcmpeq")) {
                // do nothing
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_uidivmod")) {
                // do nothing
                emit_code("static void func_%04x(state_t *state){", func_addr);
                // TODO: division by zero handling
                emit_code("     if (0 == r1) return;");
                emit_code("     uint32_t q = r0 / r1;");
                emit_code("     uint32_t r = r0 % r1;");
                emit_code("     r0 = q;");
                emit_code("     r1 = r;");
                emit_code("}");
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_idivmod")) {
                emit_code("static void func_%04x(state_t *state) {", func_addr);
                // TODO: division by zero handling
                emit_code("    if (0 == r1) return;");
                emit_code("    const int32_t q = r0_signed / r1_signed;");
                emit_code("    const int32_t r = r0_signed % r1_signed;");
                emit_code("    r0_signed = q;");
                emit_code("    r1_signed = r;");
                emit_code("}");
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ldivmod")) {
                emit_code("static void func_%04x(state_t *state){", func_addr);
                // TODO: division by zero handling
                emit_code("     if (0 == y_int64_t) return;");
                emit_code("     int64_t q = x_int64_t / y_int64_t;");
                emit_code("     int64_t r = x_int64_t % y_int64_t;");
                emit_code("     x_int64_t = q;");
                emit_code("     y_int64_t = r;");
                emit_code("}");
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_f2uiz")) {
                emit_code("static void func_%04x(state_t *state){ r0 = x_float; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_uidiv")) {
                emit_code("static void func_%04x(state_t *state){ r0 /= r1; }", func_addr);
                func_list_add_to_ignore_list(func_addr);
            } else if (!strcmp(symname, "_pause")) {
                // just an exported function
            } else if (!strcmp(symname, "_resume")) {
                // just an exported function
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else if (!strcmp(symname, "")) {
            } else {
                if (!strncmp(symname, "_", 1)) {
                    printf("unknown symbol %s\n", symname);
                    assert(0 && "not implemented");
                } else {
                    // maybe JNI entry
                    func_list_add(func_addr);
                    if (!strcmp(symname, "Java_com_example_plasma_PlasmaView_renderPlasma")) {
                        emit_code("JNIEXPORT void JNICALL Java_com_example_plasma_PlasmaView_renderPlasma(JNIEnv * env, jobject  obj, jobject bitmap,  jlong  time_ms) {");
                        emit_code("    r0 = (uint32_t)env;");
                        emit_code("    r1 = (uint32_t)obj;");
                        emit_code("    r2 = (uint32_t)bitmap;");
                        emit_code("    r13 = d_stack_start;");
                        emit_code("    store(r13, time_ms);");
                        emit_code("    store(r13 + 4, time_ms >> 32);");
                        emit_code("    func_%04x(state_t *state);", func_addr);
                        emit_code("}");
                    } else if (!strcmp(symname, "android_main")) {
                        emit_code("void android_main(struct android_app* state) {");
                        emit_code("    r0 = (uint32_t)state;");
                        emit_code("    app_dummy();");
                        emit_code("    r13 = d_stack_start;");
                        emit_code("    func_%04x(state_t *state);", func_addr);
                        emit_code("}");
                    } else if (!strcmp(symname, "app_dummy")) {
                        // android_native_app_glue.o
                    } else if (!strcmp(symname, "android_app_read_cmd")) {
                        // android_native_app_glue.o
                    } else if (!strcmp(symname, "android_app_pre_exec_cmd")) {
                        // android_native_app_glue.o
                    } else if (!strcmp(symname, "android_app_post_exec_cmd")) {
                        // android_native_app_glue.o
                    } else if (!strcmp(symname, "ANativeActivity_onCreate")) {
                        // android_native_app_glue.o
                    } else if (target_specific_symbols(symname, func_addr)) {
                        // good
                    } else if (!strcmp(symname, "")) {
                    } else if (!strcmp(symname, "")) {
                    } else if (!strcmp(symname, "")) {
                    } else if (!strcmp(symname, "")) {
                    } else if (!strcmp(symname, "")) {
                    } else if (!strcmp(symname, "")) {
                    } else if (!strcmp(symname, "")) {
                    } else {
                        printf("maybe JNI: %s\n", symname);
                        assert(0);
                    }
                }
            }
        } else if (!strcmp(sectname, ".data")) {
            if (!strcmp(symname, ".data")) {
                // do nothing
            } else if (!strcmp(symname, "__data_start")) {
                // TODO: what to do?
            } else if (!strcmp(symname, "gAppAlive")) {
                // do nothing
            } else {
                printf("unknown symbol %s\n", symname);
                assert(0 && "not implemented");
            }
        } else if (!strcmp(sectname, "*ABS*")) {
            // do nothing
        } else if (!strcmp(sectname, "*UND*")) {
            // do nothing
            // these are dynamic relocatables and they are handled in other function
        } else if (!strcmp(sectname, ".fini_array")) {
            // do nothing
        } else if (!strcmp(sectname, ".init_array")) {
            // do nothing
        } else if (!strcmp(sectname, ".data.rel.ro")) {
            // do nothing
        } else {
            printf("don't know how to process symbol of section '%s'\n", sectname);
            assert(0 && "not implemented");
        }
    }

    process_relocations(abfd, symbol_table);

    free(symbol_table);
}

static
void
generate_plt_trap_function(uint32_t func_addr)
{
    uint32_t code1 = get_word_at(func_addr);
    uint32_t code2 = get_word_at(func_addr + 4);
    uint32_t code3 = get_word_at(func_addr + 8);

    if ((code1 & 0xfffff000) == 0xe28fc000 &&
        (code2 & 0xfffff000) == 0xe28cc000 &&
        (code3 & 0xfffff000) == 0xe5bcf000)
    {
        uint32_t target_addr = arm_expand_imm12(code1 & 0xfff) + func_addr + 8;
        target_addr += arm_expand_imm12(code2 & 0xfff);
        target_addr += (code3 & 0xfff);

        if (target_addr != setjmp_func_address) {
            emit_code(" __attribute__((always_inline))");
            emit_code("static void func_%04x(state_t *state) {", func_addr);
            emit_code("    func_%04x(state);", target_addr);
            emit_code("}");
        } else {
            setjmp_plt_func_address = func_addr;
        }
    } else {
        assert(0 && "unknown plt sequence");
    }
}

static
void
declare_data_arrays(bfd *abfd)
{
    printf("declaring unified data array\n");
    FILE *fp = fopen(abfd->filename, "rb");
    assert(fp);

    struct bfd_section *sect = abfd->sections;
    uint32_t data_end = 0;

    // determine data_image size
    while (sect) {
        if (sect->flags & SEC_ALLOC) {
            uint32_t section_end = sect->vma + sect->size;
            if (data_end < section_end)
                data_end = section_end;
        }
        sect = sect->next;
    }

    unsigned char *data_image = calloc(data_end, sizeof(unsigned char));
    assert(data_image);

    // fill data_image
    sect = abfd->sections;
    while (sect) {
        if ((sect->flags & SEC_ALLOC) && (sect->flags & SEC_LOAD)) {
            fseek(fp, sect->filepos, SEEK_SET);
            fread(data_image + sect->vma, sizeof(unsigned char), sect->size, fp);
        }
        sect = sect->next;
    }

    emit_code_nonl("static unsigned char d_image[0x%x] = {", data_end);
    for (uint32_t k = 0; k < data_end; k ++) {
        if (0 != k)
            emit_code_nonl(", ");
        emit_code_nonl("0x%02x", data_image[k]);
    }
    emit_code("};");
    emit_code("static const uint32_t image_offset = (uint32_t)&d_image;");

    free(data_image);

    fclose(fp);
}

void
generate_prototypes(void)
{
    set_output_file("prototypes.inc");
    char *buf_addr = malloc(256*1024); // TODO: is that enough?
    char *buf_ptrs = malloc(256*1024); // TODO: is that enough?

    assert(buf_addr);
    assert(buf_ptrs);

    strcpy(buf_addr, "static int funclist_addr[] = { ");
    strcpy(buf_ptrs, "static void *funclist_ptr[] = { ");

    uint32_t func_pc;
    uint32_t functions_count = 0;
    while (0 != (func_pc = func_list_pop_from_done_list())) {
        char buf[1024];
        emit_code("static void func_%04x(state_t *state);", func_pc);

        if (func_pc == setjmp_plt_func_address) // do not generate wrapper for setjmp
            continue;

        if (0 != functions_count) {
            strcat(buf_addr, ", ");
            strcat(buf_ptrs, ", ");
        }

        sprintf(buf, "0x%04x", func_pc);
        strcat(buf_addr, buf);
        sprintf(buf, "&func_%04x", func_pc);
        strcat(buf_ptrs, buf);
        functions_count ++;
    }

    strcat(buf_addr, "};");
    strcat(buf_ptrs, "};");

    emit_code("static const int funclist_cnt = %d;", functions_count);
    emit_code(buf_addr);
    emit_code(buf_ptrs);

    if (setjmp_plt_func_address) {
        emit_code("#define func_%04x(notusedstate) { \\", setjmp_plt_func_address);
        emit_code("    LOG_I(\"calling setjmp(%%p)\", vv(r0)); \\");
        emit_code("    state_t saved_state = *state; \\");
        emit_code("    int32_t retval = setjmp((long int *)get_jmp_buf_address((uint32_t)aa(r0))); \\");
        emit_code("    if (retval) { \\");
        emit_code("        *state = saved_state; \\");
        emit_code("    } \\");
        emit_code("    r0_signed = retval; \\");
        emit_code("    LOG_I(\"        setjmp returned %%d\", r0); \\");
        emit_code("}");
    }

    free(buf_addr);
    free(buf_ptrs);

    close_output_file();
}

int
main(int argc, char *argv[])
{
    printf("rec started\n");

    set_output_file("code.c");

    emit_code("#define _GNU_SOURCE");
    emit_code("#include <stdint.h>");
    emit_code("#include <stdlib.h>");
    emit_code("#include <string.h>");
    emit_code("#include <setjmp.h>");
    emit_code("#include <jni.h>");
    emit_code("#include <math.h>");
    emit_code("#include <android/bitmap.h>");
    emit_code("#include <android/log.h>");
    emit_code("#include <android/looper.h>");
    emit_code("#include <android/input.h>");
    emit_code("#include <android/native_window.h>");
    emit_code("#include <android/configuration.h>");
    //emit_code("#include <android_native_app_glue.h>");
    emit_code("#include <pthread.h>");
    emit_code("#include <unistd.h>");
    emit_code("#include <sys/syscall.h>");
    emit_code("#include <sys/time.h>");
    emit_code("#include <sys/resource.h>");
    emit_code("#include <dlfcn.h>");
    emit_code("#include \"tracing.inc\"");
    emit_code("#include \"jmpbuf-table.inc\"");
    emit_code("");

    func_list_initialize();

    const char *so_name = "libplasma.so";
    if (argc > 1) {
        so_name = argv[1];
    }

    printf("so_name: %s\n", so_name);
    compute_hash_of_so(so_name);
    printf("md5: %s\n", so_md5_hash);

    bfd *abfd = bfd_openr(so_name, NULL);
    assert(abfd);
    if (!bfd_check_format(abfd, bfd_object))
        if (bfd_get_error() != bfd_error_file_ambiguously_recognized)
            assert(0 && "bfd not recognized binary");

    read_section(abfd, ".text");
    read_section(abfd, ".plt");
    read_section(abfd, ".data.rel.ro");
    read_section(abfd, ".rodata");

    declare_data_arrays(abfd);
    emit_code("#include \"registers.inc\"");
    emit_code("#include \"prototypes.inc\"");
    emit_code("#include \"storeload.inc\"");
    emit_code("#include \"findfunction.inc\"");
    determine_target_functions(abfd);

    apply_target_specific_quirks();

    while (func_list_get_count() > 0) {
        printf("functions in list: %u (%u)\n", func_list_get_count(), func_list_get_done_count());
        uint32_t func_pc = func_list_get_next();
        printf("processing function 0x%04x\n", func_pc);

        if (address_in_section(func_pc, ".plt")) {
            generate_plt_trap_function(func_pc);
        } else {
            process_function(func_pc);
        }

        func_list_mark_done(func_pc);
    }

    close_output_file();

    generate_prototypes();

    func_list_free();

    printf("done\n");
    return 0;
}
