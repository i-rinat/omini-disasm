// storeload.inc

#define aa(addr)    adjust_address(addr)

static
inline
char *
adjust_address(uint32_t addr)
{
    uint32_t k;
    for (k = 0; k < sizeof(d_section_list)/sizeof(d_section_list[0]); k ++) {
        if (d_section_list[k].begin <= addr && addr < d_section_list[k].end) {
            return d_section_list[k].ptr + addr - d_section_list[k].begin;
        }
    }

    return (char*)addr;
}

static
inline
const char *
get_section_name_by_addr(uint32_t addr)
{
    uint32_t k;
    for (k = 0; k < sizeof(d_section_list)/sizeof(d_section_list[0]); k ++) {
        if (d_section_list[k].begin <= addr && addr < d_section_list[k].end) {
            return d_section_list[k].name;
        }
    }

    return "unknown";
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
