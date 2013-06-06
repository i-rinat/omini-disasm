// storeload.inc

#define aa(addr)    adjust_address(addr)

#define ENABLE_STORELOAD_TRACING

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
#ifdef ENABLE_STORELOAD_TRACING
    LOG_I("store 0x%08x to %p (%s)", value, addr, get_section_name_by_addr(addr));
#endif
    *((uint32_t *)adjust_address(addr)) = value;
}

static
uint32_t
load(uint32_t addr)
{
#ifdef ENABLE_STORELOAD_TRACING
    LOG_I("load from %p (%s)", addr, get_section_name_by_addr(addr));
    const uint32_t res = *((uint32_t *)adjust_address(addr));
    LOG_I("load -> 0x%08x", res);
    return res;
#else
    return *((uint32_t *)adjust_address(addr));
#endif
}

static
void
store_halfword(uint32_t addr, uint32_t value)
{
#ifdef ENABLE_STORELOAD_TRACING
    LOG_I("store_halfword 0x%04x to %p (%s)", value, addr, get_section_name_by_addr(addr));
#endif
    *((uint16_t *)adjust_address(addr)) = value;
}

static
uint32_t
load_halfword(uint32_t addr)
{
#ifdef ENABLE_STORELOAD_TRACING
    LOG_I("load_halfword from %p (%s)", addr, get_section_name_by_addr(addr));
    const uint16_t res = *((uint16_t *)adjust_address(addr));
    LOG_I("load_halfword -> 0x%04x", res);
    return res;
#else
    return *((uint16_t *)adjust_address(addr));
#endif
}

static
void
store_byte(uint32_t addr, uint32_t value)
{
#ifdef ENABLE_STORELOAD_TRACING
    LOG_I("store_byte 0x%02x to %p (%s)", value, addr, get_section_name_by_addr(addr));
#endif
    *((uint8_t *)adjust_address(addr)) = value;
}

static
uint32_t
load_byte(uint32_t addr)
{
#ifdef ENABLE_STORELOAD_TRACING
    LOG_I("load_byte from %p (%s)", addr, get_section_name_by_addr(addr));
    const uint8_t res = *((uint8_t *)adjust_address(addr));
    LOG_I("load_byte -> 0x%04x", res);
    return res;
#else
    return *((uint8_t *)adjust_address(addr));
#endif
}
