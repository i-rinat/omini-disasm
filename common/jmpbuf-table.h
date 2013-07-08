// setjmp/longjmp hadling

#define JMP_BUF_TABLE_SIZE   300

static uint32_t jmp_buf_in[JMP_BUF_TABLE_SIZE];
static uint32_t jmp_buf_out[JMP_BUF_TABLE_SIZE];
static uint32_t jmp_buf_entry_count = 0;

static
uint32_t
get_jmp_buf_address(uint32_t addr)
{
    uint32_t k;
    for (k = 0; k < jmp_buf_entry_count; k ++) {
        if (jmp_buf_in[k] == addr)
            return jmp_buf_out[k];
    }

    // no entry in cache. Allocate new jmp_buf
    if (jmp_buf_entry_count >= JMP_BUF_TABLE_SIZE) {
        LOG_E("jmp_buf_entry_count >= JMP_BUF_TABLE_SIZE");
        return 0;
    }

    void *ptr = malloc(sizeof(jmp_buf));
    LOG_I("allocated %d bytes -> %p", sizeof(jmp_buf), ptr);
    jmp_buf_in[jmp_buf_entry_count] = addr;
    jmp_buf_out[jmp_buf_entry_count] = (uint32_t)ptr;
    jmp_buf_entry_count ++;

    return (uint32_t)ptr;
}
