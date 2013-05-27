// findfunction.inc

// following defined at prototypes.inc:
// int funclist_cnt;
// int funclist_addr[]
// void *funclist_ptr[]

void  find_and_call_function(uint32_t addr)
{
    int p_left = 0;
    int p_right = funclist_cnt - 1;

    if (addr < funclist_addr[0] || addr > funclist_addr[funclist_cnt-1]) {
        // that's not good.
        // TODO: report error
    }

    while (p_left != p_right) {
        const int p_middle = (p_left + p_right) / 2;
        if (addr == funclist_addr[p_middle]) {
            ((void (*)(void))(funclist_ptr[p_middle]))();
            return;
        }
        if (addr > funclist_addr[p_middle]) {
            p_left = p_middle;
        } else {
            p_right = p_middle;
        }
    }

    // TODO: report error
}
