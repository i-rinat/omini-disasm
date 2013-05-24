// storeload.inc

static
void
store(uint32_t addr, uint32_t value)
{
    if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
        d_data[addr/4 - D_DATA_START] = value;
    } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
        d_bss[addr/4 - D_BSS_START] = value;
    } else {
        *((uint32_t *)addr) = value;
    }
}

static
uint32_t
load(uint32_t addr)
{
    if (D_RODATA_START <= addr && addr < D_RODATA_START + D_RODATA_LENGTH) {
        return d_rodata[addr/4 - D_RODATA_START];
    } else if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
        return d_data[addr/4 - D_DATA_START];
    } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
        return d_bss[addr/4 - D_BSS_START];
    } else if (D_TEXT_START <= addr && addr < D_TEXT_START + D_TEXT_LENGTH) {
        return d_text[addr/4 - D_TEXT_START];
    } else {
        return *((uint32_t *)addr);
    }
}

static
void
store_halfword(uint32_t addr, uint32_t value)
{
    if ((addr & 3) == 2) {
        // shifted
        if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
            d_data[addr/4 - D_DATA_START] =
                ((value & 0xffff) << 16) | (d_data[addr/4 - D_DATA_START] & 0xffff);
        } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
            d_bss[addr/4 - D_BSS_START] =
                ((value & 0xffff) << 16) | (d_bss[addr/4 - D_BSS_START] & 0xffff);
        } else {
            *((uint16_t *)addr) = value;
        }
    } else {
        // aligned
        if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
            d_data[addr/4 - D_DATA_START] =
                (value & 0xffff) | (d_data[addr/4 - D_DATA_START] & 0xffff0000);
        } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
            d_bss[addr/4 - D_BSS_START] =
                (value & 0xffff) | (d_bss[addr/4 - D_BSS_START] & 0xffff0000);
        } else {
            *((uint16_t *)addr) = value;
        }
    }
}

static
uint32_t
load_halfword(uint32_t addr)
{
    if ((addr & 3) == 2) {
        // shifted
        if (D_RODATA_START <= addr && addr < D_RODATA_START + D_RODATA_LENGTH) {
            return d_rodata[addr/4 - D_RODATA_START] >> 16;
        } else if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
            return d_data[addr/4 - D_DATA_START] >> 16;
        } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
            return d_bss[addr/4 - D_BSS_START] >> 16;
        } else {
            return *((uint16_t *)addr);
        }
    } else {
        // aligned
        if (D_RODATA_START <= addr && addr < D_RODATA_START + D_RODATA_LENGTH) {
            return d_rodata[addr/4 - D_RODATA_START] & 0xffff;
        } else if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
            return d_data[addr/4 - D_DATA_START] & 0xffff;
        } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
            return d_bss[addr/4 - D_BSS_START] & 0xffff;
        } else {
            return *((uint16_t *)addr);
        }
    }
}
