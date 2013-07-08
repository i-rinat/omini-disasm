// storeload.h

#define vv(value)   ((void*)value)


__attribute__((always_inline))
static
void
store(uint32_t addr, uint32_t value)
{
    *((uint32_t *)addr) = value;
}

__attribute__((always_inline))
static
uint32_t
load(uint32_t addr)
{
    return *((uint32_t *)addr);
}

__attribute__((always_inline))
static
void
store_halfword(uint32_t addr, uint32_t value)
{
    value &= 0xffff;
    *((uint16_t *)addr) = (uint16_t)value;
}

__attribute__((always_inline))
static
uint32_t
load_halfword(uint32_t addr)
{
    return *((uint16_t *)addr);
}

__attribute__((always_inline))
static
void
store_byte(uint32_t addr, uint32_t value)
{
    value &= 0xff;
    *((uint8_t *)addr) = (uint8_t)value;
}

__attribute__((always_inline))
static
uint32_t
load_byte(uint32_t addr)
{
    return *((uint8_t *)addr);
}

__attribute__((always_inline))
static
double
load_double(uint32_t addr)
{
    return *((double *)addr);
}
