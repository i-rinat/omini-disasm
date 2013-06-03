// findfunction.inc

// following defined at prototypes.inc:
// int funclist_cnt;
// int funclist_addr[]
// void *funclist_ptr[]

#include "findfunction-jni.inc"

static
void
func_ffff0fc0(void)
{
    //original: typedef int (__kernel_cmpxchg_t) (int oldval, int newval, int *ptr);
    r0 = !__sync_bool_compare_and_swap_4((int *)r2, (int)r0, (int)r1);
}


static
void  find_and_call_function(uint32_t addr)
{
    int k;

    for (k = 0; k < funclist_cnt; k ++) {
        if (addr == funclist_addr[k]) {
            __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "find_and_call_function: func_%04x", addr);
            ((void (*)(void))(funclist_ptr[k]))();
            return;
        }
    }

    if (find_and_call_function_runtime((void*)addr))
        return;

    __android_log_print(ANDROID_LOG_ERROR, "libfranken", "find_and_call_function: "
                                            "function %p not found", addr);
    k = *(uint32_t *)0xdeadbeef;
}
