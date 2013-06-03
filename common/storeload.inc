// storeload.inc

#define aa(addr)    adjust_address(addr)

static
inline
char *
adjust_address(uint32_t addr)
{
    if (D_RODATA_START <= addr && addr < D_RODATA_START + D_RODATA_LENGTH) {
        return (char *)&d_rodata + addr - D_RODATA_START;
    } else if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
        return (char *)&d_data + addr - D_DATA_START;
    } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
        return (char *)&d_bss + addr - D_BSS_START;
    } else if (D_TEXT_START <= addr && addr < D_TEXT_START + D_TEXT_LENGTH) {
        return (char *)&d_text + addr - D_TEXT_START;
    } else if (D_GOT_START <= addr && addr < D_GOT_START + D_GOT_LENGTH) {
        return (char *)&d_text + addr - D_GOT_START;
    } else if (D_DYNSTR_START <= addr && addr < D_DYNSTR_START + D_DYNSTR_LENGTH) {
        return (char *)&d_dynstr + addr - D_DYNSTR_START;
    } else {
        return (char *)addr;
    }
}

static
void
store(uint32_t addr, uint32_t value)
{
    // __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "store %x to %p (before adjust)", value, addr);
    *((uint32_t *)adjust_address(addr)) = value;
}

static
uint32_t
load(uint32_t addr)
{
    // __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "load from %p (before adjust)", addr);
    return *((uint32_t *)adjust_address(addr));
}

static
void
store_halfword(uint32_t addr, uint32_t value)
{
    *((uint16_t *)adjust_address(addr)) = value;
}

static
uint32_t
load_halfword(uint32_t addr)
{
    return *((uint16_t *)adjust_address(addr));
}

static
void
store_byte(uint32_t addr, uint32_t value)
{
    *((uint8_t *)adjust_address(addr)) = value;
}

static
uint32_t
load_byte(uint32_t addr)
{
    return *((uint8_t *)adjust_address(addr));
}
