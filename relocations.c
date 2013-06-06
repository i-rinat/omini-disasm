#include "relocations.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "output.h"


static
void
process_jump_slot_relocations(arelent *relp)
{
    const char *ext_func_name = (*relp->sym_ptr_ptr)->name;

    if (!strcmp(ext_func_name, "__cxa_begin_cleanup")) {
        // do nothing
    } else if (!strcmp(ext_func_name, "memcpy")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memcpy(%%p, %%p, %%d)\", aa(r0), aa(r1), r2);");
        emit_code("    r0 = (uint32_t)memcpy((void *)aa(r0), (void *)aa(r1), (size_t)r2);");
        emit_code("    LOG_I(\"        memcpy returned %%p\", r0);");
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
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = AndroidBitmap_getInfo((JNIEnv *)aa(r0), (jobject)aa(r1), "
            "(AndroidBitmapInfo*)aa(r2));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "AndroidBitmap_lockPixels")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = AndroidBitmap_lockPixels((JNIEnv *)aa(r0), (jobject)aa(r1), "
            "(void**)aa(r2));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "AndroidBitmap_unlockPixels")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    r0 = AndroidBitmap_unlockPixels((JNIEnv *)aa(r0), (jobject)aa(r1));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "__android_log_print")) {
        // TODO: variadic functions
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        // for now nothing here
        emit_code("    __android_log_print(r0, aa(r1), aa(r2), aa(r3), aa(load(r13)), "
            "aa(load(r13+4)), aa(load(r13+8)), aa(load(r13+12)), aa(load(r13+16)), "
            "aa(load(r13+20)));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "abort")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    abort();");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "sin")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    x_double = sin(x_double);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "cos")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    x_double = cos(x_double);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "gettimeofday")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = gettimeofday((struct timeval *)aa(r0), (struct timezone *)aa(r1));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "clock_gettime")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = clock_gettime(r0_signed, (struct timespec *)aa(r1));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "memset")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memset(%%p, %%d, %%d)\", aa(r0), r1, r2);");
        emit_code("    r0 = (uint32_t)memset((void *)aa(r0), (int)r1, (size_t)r2);");
        emit_code("    LOG_I(\"        memset returned %%p\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_lock")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_mutex_lock(%%p)\", aa(r0));");
        emit_code("    r0_signed = pthread_mutex_lock((pthread_mutex_t *)aa(r0));");
        emit_code("    LOG_I(\"        pthread_mutex_lock returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_unlock")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_mutex_unlock(%%p)\", aa(r0));");
        emit_code("    r0_signed = pthread_mutex_unlock((pthread_mutex_t *)aa(r0));");
        emit_code("    LOG_I(\"        pthread_mutex_unlock returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "free")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling free(%%p)\", r0);");
        emit_code("    free((void*)aa(r0));");
        emit_code("    LOG_I(\"        free returned void\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "read")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = read(r0, (void *)aa(r1), (size_t)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_broadcast")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_broadcast((pthread_cond_t*)aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "__errno")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = (uint32_t)__errno();");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "strerror")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = (uint32_t)strerror((int)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "malloc")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling malloc(%%d)\", r0);");
        emit_code("    r0 = (uint32_t)malloc((size_t)r0);");
        emit_code("    LOG_I(\"        malloc returned %%p\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_init")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_mutex_init(%%p, %%p)\", r0, r1);");
        emit_code("    r0_signed = pthread_mutex_init((pthread_mutex_t *)aa(r0), "
                                            "(const pthread_mutexattr_t *)aa(r1));");
        emit_code("    LOG_I(\"        pthread_mutex_init returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_init")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_cond_init(%%p, %%p)\", aa(r0), aa(r1));");
        emit_code("    r0_signed = pthread_cond_init((pthread_cond_t *)aa(r0), "
                                            "(const pthread_condattr_t *)aa(r1));");
        emit_code("    LOG_I(\"        pthread_cond_init returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pipe")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pipe((int *)aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_attr_init")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_attr_init((pthread_attr_t *)aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_attr_setdetachstate")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_attr_setdetachstate((pthread_attr_t *)aa(r0), (int)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_create")) {
        emit_code("static void *proxy_38450(void *param);"); // TODO: remove hardcode
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_create(%%p, %%p, %%p, %%p)\", r0, r1, r2, r3);");
        // TODO: remove hardcode
        emit_code("    if (r2 == 0x38450) {");
        emit_code("    r0_signed = pthread_create((pthread_t *)aa(r0), "
                            "(const pthread_attr_t *)aa(r1), &proxy_38450, (void *)aa(r3));");
        emit_code("    } else {");
        emit_code("    LOG_I(\"start_routine not found\");");
        emit_code("    }");
        emit_code("    r0 = 0;"); //fake success
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_wait")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_wait((pthread_cond_t *)aa(r0), "
                                            "(pthread_mutex_t *)aa(r1));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "write")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = (uint32_t)write((int)r0, (const void *)aa(r1), (size_t)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "close")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = close((int)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_destroy")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_destroy((pthread_cond_t *)aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_destroy")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_mutex_destroy((pthread_mutex_t *)aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "raise")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = raise((int)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "longjmp")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    longjmp((long int *)aa(r0), (int)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "setjmp")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling setjmp(%%p)\", r0);");
        emit_code("    uint32_t saved_r13 = r13;");
        emit_code("    r0_signed = setjmp((long int *)aa(r0));");
        emit_code("    if (0 != r0) r13 = saved_r13;");
        emit_code("    LOG_I(\"        setjmp returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_signal")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_signal((pthread_cond_t*)aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_getspecific")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_getspecific(%%d)\", r0);");
        emit_code("    r0 = (uint32_t)pthread_getspecific((pthread_key_t)r0);");
        emit_code("    LOG_I(\"        pthread_getspecific returned %%p\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "memmove")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = (uint32_t)memmove((void*)aa(r0), (const void*)aa(r1), (size_t)aa(r2));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "snprintf")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        // TODO: snprintf variadic function
        emit_code("    ");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "syscall")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling syscall(%%d, %%p, %%p, %%p)\", r0, aa(r1), aa(r2), aa(r3));");
        // TODO: syscall variadic function
        emit_code("    r0_signed = syscall(r0, aa(r1), aa(r2), aa(r3));");
        emit_code("    LOG_I(\"        syscall returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "ceil")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    x_double = ceil(x_double);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "setpriority")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling setpriority(%%d, %%d, %%d)\", r0, r1, r2);");
        emit_code("    r0_signed = setpriority((int)r0, (int)r1, (int)r2);");
        emit_code("    LOG_I(\"        setpriority returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "qsort")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    qsort((void*)aa(r0), (size_t)r1, (size_t)r2, (void*)aa(r3));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_key_delete")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_key_delete((pthread_key_t)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_equal")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_equal(%%d, %%d)\", r0, r1);");
        emit_code("    r0_signed = pthread_equal((pthread_t)r0, (pthread_t)r1);");
        emit_code("    LOG_I(\"        pthread_equal returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "vsnprintf")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        // TODO: variadic functions
        emit_code("    ");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "dlclose")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = dlclose((void*)aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "calloc")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling calloc(%%d, %%d)\", r0, r1);");
        emit_code("    r0 = (uint32_t)calloc((size_t)r0, (size_t)r1);");
        emit_code("    LOG_I(\"        calloc returned %%p\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_key_create")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_key_create(%%p, %%p)\", aa(r0), r1);");
        // TODO: what to do if (r1 != 0) ? Need some proxy.
        emit_code("    r0_signed = pthread_key_create((pthread_key_t*)aa(r0), (void *)aa(r1));");
        emit_code("    LOG_I(\"        pthread_key_create returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "strstr")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling strstr(%%s, %%s)\", aa(r0), aa(r1));");
        emit_code("    r0 = (uint32_t)strstr((const char *)aa(r0), (const char *)aa(r1));");
        emit_code("    LOG_I(\"        strstr returned %%s\", aa(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "strncmp")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling strncmp(%%s, %%s, %d)\", aa(r0), aa(r1), r2);");
        emit_code("    r0_signed = strncmp((const char *)aa(r0), (const char *)aa(r1), (size_t)r2);");
        emit_code("    LOG_I(\"        strncmp returned %d\", r0_signed);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "dlopen")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling dlopen(%%p \\\"%%s\\\", %%d)\", aa(r0), aa(r0), r1);");
        emit_code("    r0 = (uint32_t)dlopen((const char *)aa(r0), (int)r1);");
        emit_code("    LOG_I(\"        dlopen returned %%p\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "realloc")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = (uint32_t)realloc((void *)aa(r0), (size_t)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "memcmp")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memcmp(%%p, %%p, %%d)\", aa(r0), aa(r1), r2);");
        emit_code("    r0_signed = memcmp((const void *)aa(r0), (const void *)aa(r1), (size_t)r2);");
        emit_code("    LOG_I(\"        memcmp returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "sigaction")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling fake sigaction(%%d, %%p, %%p)\", r0, aa(r1), aa(r2));");
        //emit_code("    LOG_I(\"   sigaction->sa_handler = %%p\", ((const struct sigaction *)aa(r1))->sa_handler);");
        //emit_code("    LOG_I(\"   sigaction->sa_sigaction = %%p\", ((const struct sigaction *)aa(r1))->sa_sigaction);");
        //emit_code("    LOG_I(\"   sigaction->sa_mask = %%x\", ((const struct sigaction *)aa(r1))->sa_mask);");
        //emit_code("    LOG_I(\"   sigaction->sa_flags = %%x\", ((const struct sigaction *)aa(r1))->sa_flags);");
        //emit_code("    LOG_I(\"   sigaction->sa_restorer = %%p\", ((const struct sigaction *)aa(r1))->sa_restorer);");
        //emit_code("    r0_signed = sigaction((int)r0, (const struct sigaction *)aa(r1), "
          //                  "(struct sigaction *)aa(r2));");
        emit_code("    r0 = 0;"); // fake
        emit_code("}");
    } else if (!strcmp(ext_func_name, "dlsym")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling dlsym(%%p, %%p \\\"%%s\\\")\", aa(r0), aa(r1), aa(r1));");
        emit_code("    r0 = (uint32_t)dlsym((void *)aa(r0), (const char *)aa(r1));");
        emit_code("    add_dlsym_table_entry(r0, (const char *)aa(r1));");
        emit_code("    LOG_I(\"        dlsym returned %%p\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_self")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_self()\");");
        emit_code("    r0 = (uint32_t)pthread_self();");
        emit_code("    LOG_I(\"        pthread_self returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "ceilf")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    x_float = ceilf(x_float);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "strlen")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling strlen(%%p \\\"%%s\\\")\", aa(r0), aa(r0));");
        emit_code("    r0 = (uint32_t)strlen((const char *)aa(r0));");
        emit_code("    LOG_I(\"        strlen retuned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "floorf")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    x_float = floorf(x_float);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "sched_yield")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = sched_yield();");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_join")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_join((pthread_t)r0, (void **)aa(r1));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_setspecific")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_setspecific(%%d, %%p)\", r0, aa(r1));");
        emit_code("    r0_signed = pthread_setspecific((pthread_key_t)r0, (const void *)aa(r1));");
        emit_code("    LOG_I(\"        pthread_setspecific returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    ");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    ");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    ");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    ");
        emit_code("}");
    } else {
        printf("unhandled function %s (in relocation table)\n", ext_func_name);
        assert(0 && "unhandled jump slot relocation");
    }
}

static
void
process_arm_relative_relocations(arelent *relp)
{
    const uint32_t l_addr = 0;    // base address
    emit_code("    store(%p, 0x%x + load(%p));", relp->address, l_addr + relp->addend, relp->address);
}

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

        if (!strcmp(relp->howto->name, "R_ARM_JUMP_SLOT")) {
            process_jump_slot_relocations(relp);
        } else if (!strcmp(relp->howto->name, "R_ARM_RELATIVE")) {
            // will be processed later
        }

        if (!strcmp(relp->howto->name, "UNKNOWN")) {
            assert(0 && "multiarch libbfd.a required");
        }
    }


    emit_code("__attribute__((constructor))");
    emit_code("static void do_relocations(void) {");
    for (int k = 0; k < relcount; k ++) {
        arelent *relp = relpp[k];

        if (!strcmp(relp->howto->name, "R_ARM_RELATIVE")) {
            process_arm_relative_relocations(relp);
        }
    }
    emit_code("}");


    free(relpp);
}
