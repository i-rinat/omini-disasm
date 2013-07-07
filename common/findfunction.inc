// findfunction.inc

#include "findfunction-jni.inc"
#include "findfunction-dlsym.inc"

static
void
func_ffff0fc0(state_t *state)
{
    // prototype: int __kernel_cmpxchg(int oldval, int newval, int *ptr)
    __sync_val_compare_and_swap_4((int *)r2, (int)r0, (int)r1);
    r0 = 0;
}

static
void  find_and_call_function(state_t *state, uint32_t addr)
{
    if (find_and_call_function_static_switchcase(state, addr)) {
        LOG_I("find_and_call_function: func_%04x", addr);
        return;
    }

    if (addr == 0xffff0fc0) {
        LOG_I("find_and_call_function: func_%04x", 0xffff0fc0);
        func_ffff0fc0(state);
        return;
    }

    if (find_and_call_function_runtime(state, (void*)addr))
        return;

    if (find_and_call_dlsym(state, addr))
        return;

    LOG_E("find_and_call_function: function %p (%p) not found", vv(addr), vv(addr - image_offset));
}
