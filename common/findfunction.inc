// findfunction.inc

// following defined at prototypes.inc:
// int funclist_cnt;
// int funclist_addr[]
// void *funclist_ptr[]

#include "findfunction-jni.inc"

static
void
func_ffff0fc0(state_t *state)
{
    // prototype: int __kernel_cmpxchg(int oldval, int newval, int *ptr)
    __sync_val_compare_and_swap_4((int *)aa(r2), (int)r0, (int)r1);
    r0 = 0;
}

static
void  find_and_call_function(state_t *state, uint32_t addr)
{
    int k;

    for (k = 0; k < funclist_cnt; k ++) {
        if (addr == funclist_addr[k]) {
            LOG_I("find_and_call_function: func_%04x", addr);
            ((void (*)(state_t *))(funclist_ptr[k]))(state);
            return;
        }
    }

    if (find_and_call_function_runtime(state, (void*)addr))
        return;

    LOG_E("find_and_call_function: function %p not found", addr);
}
