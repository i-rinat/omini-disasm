// findfunction.inc

// following defined at prototypes.inc:
// int funclist_cnt;
// int funclist_addr[]
// void *funclist_ptr[]

#include "findfunction-jni.inc"

static
void  find_and_call_function(uint32_t addr)
{
    int k;

    __android_log_print(ANDROID_LOG_INFO, "libfranken", "find_and_call_function: enter, 0x%08x", addr);
    for (k = 0; k < funclist_cnt; k ++) {
        if (addr == funclist_addr[k]) {
            __android_log_print(ANDROID_LOG_INFO, "libfranken", "find_and_call_function: "
                                                "found in static table, func_%04x", addr);
            ((void (*)(void))(funclist_ptr[k]))();
        }
    }

    if (find_and_call_function_runtime((void*)addr))
        return;

    __android_log_print(ANDROID_LOG_INFO, "libfranken", "find_and_call_function: "
                                            "function %p not found", addr);
}
