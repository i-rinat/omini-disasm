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
    if (!last_instruction_ended_with_return())
        emit_code("    goto label_%04x;", predicted_pc);

    assert(function_end_found());
    end_function();
    visited_bitmap_free();
    pc_stack_free();
}

static
void
process_relocations(bfd *abfd, asymbol **symbol_table)
{
    printf("processing relocations\n");

    int relsize = bfd_get_dynamic_reloc_upper_bound(abfd);
    assert(relsize >= 0);
    if (0 == relsize) {
        printf("no relocations found: relsize == 0\n");
        return;
    }

    arelent **relpp = malloc(relsize);
    assert(relpp);
    int relcount = bfd_canonicalize_dynamic_reloc(abfd, relpp, symbol_table);
    assert(relcount >= 0);
    if (0 == relcount) {
        printf("no relocations found: relcount == 0\n");
        free(relpp);
        return;
    }

    for (int k = 0; k < relcount; k ++) {
        arelent *relp = relpp[k];
        const char *ext_func_name = (*relp->sym_ptr_ptr)->name;

        if (!strcmp(relp->howto->name, "R_ARM_JUMP_SLOT")) {

            if (!strcmp(ext_func_name, "__cxa_begin_cleanup")) {
                // do nothing
            } else if (!strcmp(ext_func_name, "memcpy")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)memcpy((void *)r0, (void *)r1, r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "__cxa_finalize")) {
                // do nothing
            } else if (!strcmp(ext_func_name, "__cxa_type_match")) {
                // do nothing
            } else if (!strcmp(ext_func_name, "__cxa_atexit")) {
                // do nothing
            } else if (!strcmp(ext_func_name, "__gnu_Unwind_Find_exidx")) {
                // do nothing
            } else if (!strcmp(ext_func_name, "AndroidBitmap_getInfo")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = AndroidBitmap_getInfo((JNIEnv *)r0, (jobject)r1, "
                    "(AndroidBitmapInfo*)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AndroidBitmap_lockPixels")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = AndroidBitmap_lockPixels((JNIEnv *)r0, (jobject)r1, "
                    "(void**)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AndroidBitmap_unlockPixels")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = AndroidBitmap_unlockPixels((JNIEnv *)r0, (jobject)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "__android_log_print")) {
                // TODO: variadic functions
                emit_code("static void func_%04x() {", relp->address);
                // for now nothing here
                emit_code("}");
            } else if (!strcmp(ext_func_name, "abort")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    abort();");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "sin")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.x_double = sin(reg.x_double);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "cos")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.x_double = cos(reg.x_double);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "gettimeofday")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = gettimeofday((struct timeval *)r0, (struct timezone *)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "ANativeWindow_lock")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = ANativeWindow_lock((ANativeWindow*)r0, "
                    "(ANativeWindow_Buffer*)r1, (ARect*)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "clock_gettime")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = clock_gettime(reg.r0_signed, (struct timespec *)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "ANativeWindow_unlockAndPost")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = ANativeWindow_unlockAndPost((ANativeWindow*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AInputEvent_getType")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AInputEvent_getType((const AInputEvent*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AKeyEvent_getAction")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AKeyEvent_getAction((const AInputEvent*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AKeyEvent_getKeyCode")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AKeyEvent_getKeyCode((const AInputEvent*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AKeyEvent_getMetaState")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AKeyEvent_getKeyCode((const AInputEvent*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "memset")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)memset((void *)r0, (int)r1, (size_t)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "ALooper_pollAll")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = ALooper_pollAll(reg.r0_signed, (int*)r1, (int*)r2, (void**)r3);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_mutex_lock")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_mutex_lock((pthread_mutex_t *)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_mutex_unlock")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_mutex_unlock((pthread_mutex_t *)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "free")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    free((void*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "read")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = read(r0, (void *)r1, (size_t)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getLanguage")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    AConfiguration_getLanguage((AConfiguration*)r0, (char*)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getCountry")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    AConfiguration_getCountry((AConfiguration*)r0, (char*)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getMcc")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getMcc((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getMnc")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getMnc((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getOrientation")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getOrientation((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getTouchscreen")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getTouchscreen((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getDensity")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getDensity((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getKeyboard")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getKeyboard((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getNavigation")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getNavigation((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getKeysHidden")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getKeysHidden((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getNavHidden")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getNavHidden((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getSdkVersion")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getSdkVersion((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getScreenSize")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getScreenSize((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getScreenLong")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getScreenLong((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getUiModeType")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getUiModeType((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_getUiModeNight")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AConfiguration_getUiModeNight((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AInputQueue_detachLooper")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    AInputQueue_detachLooper((AInputQueue*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AInputQueue_attachLooper")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    const uint32_t param5 = load(r13);");
                emit_code("    AInputQueue_attachLooper((AInputQueue*)r0, (ALooper*)r1, (int)r2, "
                                                "(ALooper_callbackFunc)r3, (void *)param5);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_cond_broadcast")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_cond_broadcast((pthread_cond_t*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_fromAssetManager")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    AConfiguration_fromAssetManager((AConfiguration*)r0, (AAssetManager*)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_delete")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    AConfiguration_delete((AConfiguration*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AInputQueue_getEvent")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AInputQueue_getEvent((AInputQueue*)r0, (AInputEvent**)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AInputQueue_preDispatchEvent")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = AInputQueue_preDispatchEvent((AInputQueue*)r0, (AInputEvent*)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AInputQueue_finishEvent")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    AInputQueue_finishEvent((AInputQueue*)r0, (AInputEvent*)r1, (int)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "__errno")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)__errno();");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "strerror")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)strerror((int)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "AConfiguration_new")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)AConfiguration_new();");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "ALooper_prepare")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)ALooper_prepare((int)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "ALooper_addFd")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    const uint32_t param5 = load(r13);");
                emit_code("    const uint32_t param6 = load(r13+4);");
                emit_code("    reg.r0_signed = ALooper_addFd((ALooper*)r0, (int)r1, (int)r2, "
                            "(int)r3, (ALooper_callbackFunc)param5, (void*)param6);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "malloc")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)malloc((size_t)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_mutex_init")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_mutex_init((pthread_mutex_t *)r0, "
                                                    "(const pthread_mutexattr_t *)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_cond_init")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_cond_init((pthread_cond_t *)r0, "
                                                    "(const pthread_condattr_t *)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pipe")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pipe((int *)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_attr_init")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_attr_init((pthread_attr_t *)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_attr_setdetachstate")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_attr_setdetachstate((pthread_attr_t *)r0, "
                                                            "(int)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_create")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_create((pthread_t *)r0, "
                                    "(const pthread_attr_t *)r1, (void *)r2, (void *)r3);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_cond_wait")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_cond_wait((pthread_cond_t *)r0, "
                                                    "(pthread_mutex_t *)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "write")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = (uint32_t)write((int)r0, (const void *)r1, (size_t)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "close")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = close((int)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_cond_destroy")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_cond_destroy((pthread_cond_t *)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_mutex_destroy")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_mutex_destroy((pthread_mutex_t *)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "raise")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = raise((int)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "longjmp")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    longjmp((jmp_buf)r0, (int)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "setjmp")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = setjmp((jmp_buf)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_cond_signal")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_cond_signal((pthread_cond_t*)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_getspecific")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)pthread_getspecific((pthread_key_t)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "memmove")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    r0 = (uint32_t)memmove((void*)r0, (const void*)r1, (size_t)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "snprintf")) {
                emit_code("static void func_%04x() {", relp->address);
                // TODO: snprintf variadic function
                emit_code("    ");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "syscall")) {
                emit_code("static void func_%04x() {", relp->address);
                // TODO: syscall variadic function
                emit_code("    reg.r0_signed = syscall(r0, r1, r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "ceil")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.x_double = ceil(reg.x_double);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "setpriority")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = setpriority((int)r0, (int)r1, (int)r2);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "qsort")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    qsort((void*)r0, (size_t)r1, (size_t)r2, (void*)r3);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_key_delete")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_key_delete((pthread_key_t)r0);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "pthread_equal")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    reg.r0_signed = pthread_equal((pthread_t)r0, (pthread_t)r1);");
                emit_code("}");
            } else if (!strcmp(ext_func_name, "")) {
                emit_code("static void func_%04x() {", relp->address);
                emit_code("    ");
                emit_code("}");
            } else {
                printf("unhandled function %s\n", ext_func_name);
                assert(0 && "unhandled import");
            }
        }

        if (!strcmp(relp->howto->name, "UNKNOWN")) {
            assert(0 && "multiarch libbfd.a required");
        }
    }

    free(relpp);
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
                emit_code("static void func_%04x() { r0 = reg.x_double == reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmplt")) {
                emit_code("static void func_%04x() { r0 = reg.x_double < reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmple")) {
                emit_code("static void func_%04x() { r0 = reg.x_double <= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmpge")) {
                emit_code("static void func_%04x() { r0 = reg.x_double >= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dcmpgt")) {
                emit_code("static void func_%04x() { r0 = reg.x_double > reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__cmpdf2")) {
                // this is some underlying stuff, skip it
            } else if (!strcmp(symname, "__eqdf2")) {
                // alias for __cmpdf2
            } else if (!strcmp(symname, "__floatdidf")) {
                // alias for __aeabi_l2d
            } else if (!strcmp(symname, "__aeabi_l2d")) {
                emit_code("static void func_%04x() { reg.x_double = reg.x_uint64_t; }", func_addr);
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
                emit_code("static void func_%04x() { reg.x_double = reg.r0_signed; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ui2d")) {
                emit_code("static void func_%04x() { reg.x_double = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dsub")) {
                emit_code("static void func_%04x() { reg.x_double = reg.x_double - reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_drsub")) {
                emit_code("static void func_%04x() { reg.x_double = reg.y_double - reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__extendsfdf2")) {
                emit_code("static void func_%04x() { reg.x_double = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ddiv")) {
                emit_code("static void func_%04x() { reg.x_double /= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__divdf3")) {
                emit_code("static void func_%04x() { reg.x_double /= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__adddf3")) {
                emit_code("static void func_%04x(){ reg.x_double += reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__subdf3")) {
                emit_code("static void func_%04x(){ reg.x_double -= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dmul")) {
                emit_code("static void func_%04x(){ reg.x_double *= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__muldf3")) {
                emit_code("static void func_%04x(){ reg.x_double *= reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_dadd")) {
                emit_code("static void func_%04x(){ reg.x_double += reg.y_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatunsidf")) {
                emit_code("static void func_%04x(){ reg.x_double = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__fixdfsi")) {
                emit_code("static void func_%04x(){ reg.r0_signed = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ul2d")) {
                emit_code("static void func_%04x(){ reg.x_double = reg.x_uint64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatundidf")) {
                emit_code("static void func_%04x(){ reg.x_double = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__nedf2")) {
                emit_code("static void func_%04x(){ r0 = (reg.x_double != reg.y_double); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__gedf2")) {
                emit_code("static void func_%04x(){ r0 = (reg.x_double >= reg.y_double); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_d2iz")) {
                emit_code("static void func_%04x(){ reg.r0_signed = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_f2d")) {
                emit_code("static void func_%04x(){ reg.x_double = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__gnu_uldivmod_helper")) {
                // do nothing
            } else if (!strcmp(symname, "__gnu_ldivmod_helper")) {
                // do nothing
            } else if (!strcmp(symname, "__divdi3")) {
                emit_code("static void func_%04x(){ reg.x_int64_t /= reg.y_int64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__divsf3")) {
                emit_code("static void func_%04x(){ reg.x_float /= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__fixsfsi")) {
                emit_code("static void func_%04x(){ reg.x_signed = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fadd")) {
                emit_code("static void func_%04x(){ reg.x_float += reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_frsub")) {
                emit_code("static void func_%04x(){ reg.x_float = reg.y_float - reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__subsf3")) {
                emit_code("static void func_%04x(){ reg.x_float -= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__mulsf3")) {
                emit_code("static void func_%04x(){ reg.x_float *= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpeq")) {
                emit_code("static void func_%04x() { r0 = reg.x_float == reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmplt")) {
                emit_code("static void func_%04x() { r0 = reg.x_float < reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmple")) {
                emit_code("static void func_%04x() { r0 = reg.x_float <= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpge")) {
                emit_code("static void func_%04x() { r0 = reg.x_float >= reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_fcmpgt")) {
                emit_code("static void func_%04x() { r0 = reg.x_float > reg.y_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_uldivmod")) {
                // do nothing
            } else if (!strcmp(symname, "__udivsi3")) {
                emit_code("static void func_%04x(){ r0 /= r1; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_i2f")) {
                emit_code("static void func_%04x(){ reg.x_float = reg.r0_signed; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__lesf2")) {
                emit_code("static void func_%04x(){ r0 = (reg.x_float <= reg.y_float); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__gtsf2")) {
                emit_code("static void func_%04x(){ r0 = (reg.x_float > reg.y_float); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__nesf2")) {
                emit_code("static void func_%04x(){ r0 = (reg.x_float != reg.y_float); }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__fixunsdfsi")) {
                emit_code("static void func_%04x(){ reg.r0_signed = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__udivdi3")) {
                // do nothing
            } else if (!strcmp(symname, "__aeabi_d2f")) {
                emit_code("static void func_%04x(){ reg.x_float = reg.x_double; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatdisf")) {
                emit_code("static void func_%04x(){ reg.x_float = reg.x_int64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_idiv")) {
                emit_code("static void func_%04x(){ reg.r0_signed /= reg.r1_signed; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__floatundisf")) {
                emit_code("static void func_%04x(){ reg.x_float = reg.x_uint64_t; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_ui2f")) {
                emit_code("static void func_%04x(){ reg.x_float = r0; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__div0")) {
                // please don't divide by zero
            } else if (!strcmp(symname, "__aeabi_ldiv0")) {
                // please don't divide by zero
            } else if (!strcmp(symname, "__aeabi_idiv0")) {
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
                emit_code("static void func_%04x(){ r0 = reg.x_float; }", func_addr);
                func_list_add_to_done_list(func_addr);
            } else if (!strcmp(symname, "__aeabi_uidiv")) {
                emit_code("static void func_%04x(){ r0 /= r1; }", func_addr);
                func_list_add_to_done_list(func_addr);
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
                        emit_code("    r3 = (uint32_t)time_ms;");
                        emit_code("    r13 = d_stack_start;");
                        emit_code("    func_%04x();", func_addr);
                        emit_code("}");
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

        emit_code("static inline void func_%04x() {", func_addr);
        emit_code("    func_%04x();", target_addr);
        emit_code("}");
    } else {
        assert(0 && "unknown plt sequence");
    }
}

static
void
declare_data_arrays(bfd *abfd)
{
    printf("declaring data arrays\n");
    FILE *fp = fopen(abfd->filename, "rb");
    assert(fp);
    struct bfd_section *sect = abfd->sections;

    while (sect) {
        if (!strcmp(sect->name, ".rodata")) {
            emit_code("#define D_RODATA_START 0x%x", sect->vma);
            emit_code("#define D_RODATA_LENGTH 0x%x", sect->size);
            emit_code("const uint32_t d_rodata[%d] = {", sect->size / 4);
            uint32_t *buf = malloc(sect->size);
            assert(buf);
            fseek(fp, sect->filepos, SEEK_SET);
            fread(buf, 4, sect->size / 4, fp);
            for (unsigned int k = 0; k < sect->size / 4; k ++) {
                if (0 == k)
                    emit_code("0x%x", buf[k]);
                else
                    emit_code(", 0x%x", buf[k]);
            }
            emit_code("};");
            free(buf);
        } else if (!strcmp(sect->name, ".text")) {
            // this should not be here
            // TODO: invent register tracing to avoid including whole .text section
            emit_code("#define D_TEXT_START 0x%x", sect->vma);
            emit_code("#define D_TEXT_LENGTH 0x%x", sect->size);
            emit_code("const uint32_t d_text[%d] = {", sect->size / 4);
            uint32_t *buf = malloc(sect->size);
            assert(buf);
            fseek(fp, sect->filepos, SEEK_SET);
            fread(buf, 4, sect->size / 4, fp);
            for (unsigned int k = 0; k < sect->size / 4; k ++) {
                if (0 == k)
                    emit_code("0x%x", buf[k]);
                else
                    emit_code(", 0x%x", buf[k]);
            }
            emit_code("};");
            free(buf);
        } else if (!strcmp(sect->name, ".data")) {
            emit_code("#define D_DATA_START 0x%x", sect->vma);
            emit_code("#define D_DATA_LENGTH 0x%x", sect->size);
            emit_code("uint32_t d_data[%d] = {", sect->size / 4);
            uint32_t *buf = malloc(sect->size);
            assert(buf);
            fseek(fp, sect->filepos, SEEK_SET);
            fread(buf, 4, sect->size / 4, fp);
            for (unsigned int k = 0; k < sect->size / 4; k ++) {
                if (0 == k)
                    emit_code("0x%x", buf[k]);
                else
                    emit_code(", 0x%x", buf[k]);
            }
            emit_code("};");
            free(buf);
        } else if (!strcmp(sect->name, ".bss")) {
            emit_code("#define D_BSS_START 0x%x", sect->vma);
            emit_code("#define D_BSS_LENGTH 0x%x", sect->size);
            emit_code("uint32_t d_bss[%d];", sect->size / 4);
        }

        sect = sect->next;
    }

    fclose(fp);

    // define stack
    emit_code("#define D_STACK_LENGTH   2000");
    emit_code("uint32_t d_stack[D_STACK_LENGTH];");
    emit_code("const uint32_t d_stack_start = (uint32_t)(&(d_stack[D_STACK_LENGTH - 32]));");
}

int
main(int argc, char *argv[])
{
    printf("rec started\n");

    set_output_file("code.c");

    emit_code("#include <stdint.h>");
    emit_code("#include <stdlib.h>");
    emit_code("#include <string.h>");
    emit_code("#include <jni.h>");
    emit_code("#include <math.h>");
    emit_code("#include <android/bitmap.h>");
    emit_code("#include \"registers.inc\"");
    emit_code("#include \"prototypes.inc\"");
    emit_code("#include \"findfunction.inc\"");
    emit_code("");

    func_list_initialize();

    const char *so_name = "libplasma.so";
    if (argc > 1) {
        so_name = argv[1];
    }

    printf("so_name: %s\n", so_name);
    bfd *abfd = bfd_openr(so_name, NULL);
    assert(abfd);
    if (!bfd_check_format(abfd, bfd_object))
        if (bfd_get_error() != bfd_error_file_ambiguously_recognized)
            assert(0 && "bfd not recognized binary");

    read_section(abfd, ".text");
    read_section(abfd, ".plt");

    declare_data_arrays(abfd);
    emit_code("#include \"storeload.inc\"");
    determine_target_functions(abfd);

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

    // generate prototypes
    set_output_file("prototypes.inc");
    char *buf_addr = malloc(256*1024); // TODO: is that enough?
    char *buf_ptrs = malloc(256*1024); // TODO: is that enough?

    assert(buf_addr);
    assert(buf_ptrs);

    strcpy(buf_addr, "int funclist_addr[] = { ");
    strcpy(buf_ptrs, "void *funclist_ptr[] = { ");

    uint32_t func_pc;
    uint32_t functions_count = 0;
    while (0 != (func_pc = func_list_pop_from_done_list())) {
        char buf[1024];
        emit_code("static void func_%04x();", func_pc);

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

    emit_code("const int funclist_cnt = %d;", functions_count);
    emit_code(buf_addr);
    emit_code(buf_ptrs);
    free(buf_addr);
    free(buf_ptrs);

    close_output_file();


    func_list_free();

    printf("done\n");
    return 0;
}
