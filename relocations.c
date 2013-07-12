#include "relocations.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "output.h"
#include "pc-stack.h"

uint32_t setjmp_func_address = 0;
uint32_t setjmp_plt_func_address = 0;

static
void
process_jump_slot_relocations(arelent *relp)
{
    const char *ext_func_name = (*relp->sym_ptr_ptr)->name;

    if (!strcmp(ext_func_name, "__cxa_begin_cleanup")) {
        // do nothing
    } else if (!strcmp(ext_func_name, "memcpy")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memcpy(%%p, %%p, %%d)\", vv(r0), vv(r1), r2);");
        emit_code("    r0 = (uint32_t)memcpy((void *)r0, (void *)r1, (size_t)r2);");
        emit_code("    LOG_I(\"        memcpy returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "__cxa_finalize")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    // this function intentionally left blank");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "__cxa_type_match")) {
        // do nothing
    } else if (!strcmp(ext_func_name, "__cxa_atexit")) {
        // do nothing
    } else if (!strcmp(ext_func_name, "__gnu_Unwind_Find_exidx")) {
        // do nothing
    } else if (!strcmp(ext_func_name, "AndroidBitmap_getInfo")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = AndroidBitmap_getInfo((JNIEnv *)r0, (jobject)r1, (AndroidBitmapInfo*)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "AndroidBitmap_lockPixels")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = AndroidBitmap_lockPixels((JNIEnv *)r0, (jobject)r1, (void**)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "AndroidBitmap_unlockPixels")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = AndroidBitmap_unlockPixels((JNIEnv *)r0, (jobject)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "__android_log_print")) {
        // TODO: variadic functions
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        // for now nothing here
        emit_code("    __android_log_print(r0, (const char *)r1, (const char *)r2, r3, load(r13), "
            "load(r13+4), load(r13+8), load(r13+12), load(r13+16), "
            "load(r13+20));");
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
        emit_code("    r0 = gettimeofday((struct timeval *)r0, (struct timezone *)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "clock_gettime")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = clock_gettime(r0_signed, (struct timespec *)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "memset")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memset(%%p, %%d, %%d)\", vv(r0), r1, r2);");
        emit_code("    r0 = (uint32_t)memset((void *)r0, (int)r1, (size_t)r2);");
        emit_code("    LOG_I(\"        memset returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_lock")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_mutex_lock(%%p)\", vv(r0));");
        emit_code("    r0_signed = pthread_mutex_lock((pthread_mutex_t *)r0);");
        emit_code("    LOG_I(\"        pthread_mutex_lock returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_unlock")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_mutex_unlock(%%p)\", vv(r0));");
        emit_code("    r0_signed = pthread_mutex_unlock((pthread_mutex_t *)r0);");
        emit_code("    LOG_I(\"        pthread_mutex_unlock returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "free")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling free(%%p)\", vv(r0));");
        emit_code("    free((void*)r0);");
        emit_code("    LOG_I(\"        free returned void\");");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "read")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = read(r0, (void *)r1, (size_t)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_broadcast")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_broadcast((pthread_cond_t*)r0);");
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
        emit_code("    LOG_I(\"        malloc returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_init")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_mutex_init(%%p, %%p)\", vv(r0), vv(r1));");
        emit_code("    r0_signed = pthread_mutex_init((pthread_mutex_t *)r0, "
                                            "(const pthread_mutexattr_t *)r1);");
        emit_code("    LOG_I(\"        pthread_mutex_init returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_init")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_cond_init(%%p, %%p)\", vv(r0), vv(r1));");
        emit_code("    r0_signed = pthread_cond_init((pthread_cond_t *)r0, "
                                            "(const pthread_condattr_t *)r1);");
        emit_code("    LOG_I(\"        pthread_cond_init returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pipe")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pipe((int *)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_attr_init")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_attr_init((pthread_attr_t *)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_attr_setdetachstate")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_attr_setdetachstate((pthread_attr_t *)r0, (int)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_create")) {
        emit_code("static void *proxy_38450(void *param);"); // TODO: remove hardcode
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_create(%%p, %%p, %%p, %%p)\", vv(r0), vv(r1), vv(r2), vv(r3));");
        // TODO: remove hardcode
        emit_code("    if (r2 == image_offset + 0x38450) {");
        emit_code("    r0_signed = pthread_create((pthread_t *)r0, "
                            "(const pthread_attr_t *)r1, &proxy_38450, (void *)r3);");
        emit_code("    } else {");
        emit_code("        LOG_E(\"pthread_create: start_routine (%%p) (%%p) not found\", vv(r2), vv(r2-image_offset));");
        emit_code("    }");
        emit_code("    r0 = 0;"); //fake success
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_wait")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_wait((pthread_cond_t *)r0, "
                                            "(pthread_mutex_t *)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "write")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = (uint32_t)write((int)r0, (const void *)r1, (size_t)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "close")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = close((int)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_cond_destroy")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_destroy((pthread_cond_t *)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_mutex_destroy")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_mutex_destroy((pthread_mutex_t *)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "raise")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = raise((int)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "longjmp")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling longjmp(%%p, %%d)\", vv(r0), r1);");
        // pointer to actual jmp_buf was stored at r0
        emit_code("    void *p = (void*)load(r0);");
        emit_code("    longjmp(p, (int)r1);");
        emit_code("    LOG_I(\"        longjmp should not return\");");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "setjmp")) {
        // define function as macro in order to force inlining setjmp
        // later, in prototypes.h
        setjmp_func_address = relp->address;

    } else if (!strcmp(ext_func_name, "pthread_cond_signal")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_cond_signal((pthread_cond_t*)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_getspecific")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_getspecific(%%d)\", r0);");
        emit_code("    r0 = (uint32_t)pthread_getspecific((pthread_key_t)r0);");
        emit_code("    LOG_I(\"        pthread_getspecific returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "memmove")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memmove(%%p, %%p, %%d)\", vv(r0), vv(r1), r2);");
        emit_code("    r0 = (uint32_t)memmove((void*)r0, (const void*)r1, (size_t)r2);");
        emit_code("    LOG_I(\"        memmove returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "snprintf")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_E(\"calling %s\");", ext_func_name);
        // TODO: snprintf variadic function
        emit_code("    ");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "syscall")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling syscall(%%d, %%p, %%p, %%p)\", r0, vv(r1), vv(r2), vv(r3));");
        // TODO: syscall variadic function
        emit_code("    r0_signed = syscall(r0, r1, r2, r3);");
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
        // TODO: qsort wants function
        emit_code("    LOG_E(\"calling %s\");", ext_func_name);
        emit_code("    qsort((void*)r0, (size_t)r1, (size_t)r2, (void*)r3);");
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
        emit_code("    LOG_E(\"calling %s\");", ext_func_name);
        // TODO: variadic functions
        emit_code("    ");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "dlclose")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = dlclose((void*)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "calloc")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling calloc(%%d, %%d)\", r0, r1);");
        emit_code("    r0 = (uint32_t)calloc((size_t)r0, (size_t)r1);");
        emit_code("    LOG_I(\"        calloc returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_key_create")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_key_create(%%p, %%p)\", vv(r0), vv(r1));");
        // TODO: what to do if (r1 != 0) ? Need some proxy.
        emit_code("    r0_signed = pthread_key_create((pthread_key_t*)r0, (void *)r1);");
        emit_code("    LOG_I(\"        pthread_key_create returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "strstr")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling strstr(%%p \\\"%%s\\\", %%p, \\\"%%s\\\")\", vv(r0), r0, vv(r1), r1);");
        emit_code("    r0 = (uint32_t)strstr((const char *)r0, (const char *)r1);");
        emit_code("    LOG_I(\"        strstr returned %%p \\\"%%s\\\"\", vv(r0), r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "strncmp")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling strncmp(%%p \\\"%%s\\\", %%p \\\"%%s\\\", %%d)\", vv(r0), r0, vv(r1), r1, r2);");
        emit_code("    r0_signed = strncmp((const char *)r0, (const char *)r1, (size_t)r2);");
        emit_code("    LOG_I(\"        strncmp returned %%d\", r0_signed);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "dlopen")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling dlopen(%%p \\\"%%s\\\", %%d)\", vv(r0), r0, r1);");
        emit_code("    r0 = (uint32_t)dlopen((const char *)r0, (int)r1);");
        emit_code("    LOG_I(\"        dlopen returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "realloc")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0 = (uint32_t)realloc((void *)r0, (size_t)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "memcmp")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memcmp(%%p, %%p, %%d)\", vv(r0), vv(r1), r2);");
        emit_code("    r0_signed = memcmp((const void *)r0, (const void *)r1, (size_t)r2);");
        emit_code("    LOG_I(\"        memcmp returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "sigaction")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling fake sigaction(%%d, %%p, %%p)\", r0, vv(r1), vv(r2));");
        //emit_code("    LOG_I(\"   sigaction->sa_handler = %%p\", ((const struct sigaction *)r1)->sa_handler);");
        //emit_code("    LOG_I(\"   sigaction->sa_sigaction = %%p\", ((const struct sigaction *)r1)->sa_sigaction);");
        //emit_code("    LOG_I(\"   sigaction->sa_mask = %%lx\", ((const struct sigaction *)r1)->sa_mask);");
        //emit_code("    LOG_I(\"   sigaction->sa_flags = %%lx\", ((const struct sigaction *)r1)->sa_flags);");
        //emit_code("    LOG_I(\"   sigaction->sa_restorer = %%p\", ((const struct sigaction *)r1)->sa_restorer);");
        //emit_code("    r0_signed = sigaction((int)r0, (const struct sigaction *)r1, "
        //                    "(struct sigaction *)r2);");
        emit_code("    r0 = 0;"); // fake
        emit_code("}");
    } else if (!strcmp(ext_func_name, "dlsym")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling dlsym(%%p, %%p \\\"%%s\\\")\", vv(r0), vv(r1), r1);");
        emit_code("    r0 = (uint32_t)dlsym((void *)r0, (const char *)r1);");
        emit_code("    add_dlsym_table_entry(r0, (const char *)r1);");
        emit_code("    LOG_I(\"        dlsym returned %%p\", vv(r0));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_self")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_self()\");");
        emit_code("    r0 = (uint32_t)pthread_self();");
        emit_code("    LOG_I(\"        pthread_self returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "ceilf")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling ceilf(%%f)\", x_float);");
        emit_code("    x_float = ceilf(x_float);");
        emit_code("    LOG_I(\"        ceilf returned %%f\", x_float);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "strlen")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling strlen(%%p \\\"%%s\\\")\", vv(r0), r0);");
        emit_code("    r0 = (uint32_t)strlen((const char *)r0);");
        emit_code("    LOG_I(\"        strlen retuned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "floorf")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling floorf(%%f)\", x_float);");
        emit_code("    x_float = floorf(x_float);");
        emit_code("    LOG_I(\"        floorf returned %%f\", x_float);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "sched_yield")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = sched_yield();");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_join")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling %s\");", ext_func_name);
        emit_code("    r0_signed = pthread_join((pthread_t)r0, (void **)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pthread_setspecific")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling pthread_setspecific(%%d, %%p)\", r0, vv(r1));");
        emit_code("    r0_signed = pthread_setspecific((pthread_key_t)r0, (const void *)r1);");
        emit_code("    LOG_I(\"        pthread_setspecific returned %%d\", r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glVertexPointer")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glVertexPointer((GLint)r0, (GLenum)r1, (GLsizei)r2, (const GLvoid *)r3);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glColorPointer")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glColorPointer((GLint)r0, (GLenum)r1, (GLsizei)r2, (const GLvoid *)r3);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glNormalPointer")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glNormalPointer((GLenum)r0, (GLsizei)r1, (const GLvoid *)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glEnableClientState")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glEnableClientState((GLenum)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glDisableClientState")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glDisableClientState((GLenum)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glDrawArrays")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glDrawArrays((GLenum)r0, (GLint)r1, (GLsizei)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "pow")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    x_double = pow(x_double, y_double);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glDisable")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glDisable((GLenum)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glEnable")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glEnable((GLenum)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glBlendFunc")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glBlendFunc((GLenum)r0, (GLenum)r1);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glColor4x")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glColor4x((GLfixed)r0, (GLfixed)r1, (GLfixed)r2, (GLfixed)r3);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glMatrixMode")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glMatrixMode((GLenum)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glLoadIdentity")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glLoadIdentity();");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glShadeModel")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glShadeModel((GLenum)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "tan")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    x_double = tan(x_double);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glFrustumx")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glFrustumx((GLfixed)r0, (GLfixed)r1, (GLfixed)r2, (GLfixed)r3, "
                    "(GLfixed)load(r13), (GLfixed)load(r13 + 4));");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glViewport")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glViewport((GLint)r0, (GLint)r1, (GLsizei)r2, (GLsizei)r3);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glClearColorx")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glClearColorx((GLfixed)r0, (GLfixed)r1, (GLfixed)r2, (GLfixed)r3);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glClear")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glClear((GLbitfield)r0);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glLightxv")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glLightxv((GLenum)r0, (GLenum)r1, (const GLfixed *)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glMaterialxv")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glMaterialxv((GLenum)r0, (GLenum)r1, (const GLfixed *)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glMaterialx")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glMaterialx((GLenum)r0, (GLenum)r1, (GLfixed)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glScalex")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glScalex((GLfixed)r0, (GLfixed)r1, (GLfixed)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glPushMatrix")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glPushMatrix();");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glTranslatex")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glTranslatex((GLfixed)r0, (GLfixed)r1, (GLfixed)r2);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glRotatex")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glRotatex((GLfixed)r0, (GLfixed)r1, (GLfixed)r2, (GLfixed)r3);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glPopMatrix")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glPopMatrix();");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "sqrt")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    x_double = sqrt(x_double);");
        emit_code("}");
    } else if (!strcmp(ext_func_name, "glMultMatrixx")) {
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    glMultMatrixx((const GLfixed *)r0);");
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
    emit_code("    store(image_offset + %p, image_offset + %uu + load(image_offset + %p));",
        relp->address, relp->addend, relp->address);
}

static
void
process_glob_dat_relocations(arelent *relp)
{
    const char *ext_func_name = (*relp->sym_ptr_ptr)->name;

    if (!strcmp(ext_func_name, "memset")) {
        emit_code("// process_glob_dat_relocations");
        emit_code("static void func_%04x(state_t *state) {", relp->address);
        emit_code("    LOG_I(\"calling memset(%%p, %%d, %%d)\", vv(r0), r1, r2);");
        emit_code("    r0 = (uint32_t)memset((void *)r0, (int)r1, (size_t)r2);");
        emit_code("    LOG_I(\"        memset returned %%p\", vv(r0));");
        emit_code("}");
        func_list_add(relp->address);
        func_list_mark_done(relp->address);
    } else if (!strcmp(ext_func_name, "__cxa_call_unexpected")) {
    } else if (!strcmp(ext_func_name, "__gnu_Unwind_Find_exidx")) {
    } else if (!strcmp(ext_func_name, "")) {
    } else if (!strcmp(ext_func_name, "")) {
    } else {
        printf("not handled glob dat relocation: %s\n", ext_func_name);
        assert(0);
    }
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
        } else if (!strcmp(relp->howto->name, "R_ARM_GLOB_DAT")) {
            process_glob_dat_relocations(relp);
        } else {
            printf("not handled relocation of type %s\n", relp->howto->name);
            assert(0);
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
        } else if (!strcmp(relp->howto->name, "R_ARM_GLOB_DAT")) {
            emit_code("    store(image_offset + %p, image_offset + %p);", relp->address, relp->address);
        }

    }
    emit_code("}");


    free(relpp);
}
