// storeload.inc

static
void
store(uint32_t addr, uint32_t value)
{
    if (D_RODATA_START <= addr && addr < D_RODATA_START + D_RODATA_LENGTH) {
        // failure.
        // TODO: report error
    } else if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
        d_data[addr/4 - D_DATA_START] = value;
    } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
        d_bss[addr/4 - D_BSS_START] = value;
    } else {
        // failure.
        // TODO: report error
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
    } else {
        // failure.
        // TODO: report error
    }
}

static
void
store_halfword(uint32_t addr, uint32_t value)
{
    if ((addr & 3) == 2) {
        // shifted
        if (D_RODATA_START <= addr && addr < D_RODATA_START + D_RODATA_LENGTH) {
            // failure.
            // TODO: report error
        } else if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
            d_data[addr/4 - D_DATA_START] =
                ((value & 0xffff) << 16) | (d_data[addr/4 - D_DATA_START] & 0xffff);
        } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
            d_bss[addr/4 - D_BSS_START] =
                ((value & 0xffff) << 16) | (d_bss[addr/4 - D_BSS_START] & 0xffff);
        } else {
            // failure.
            // TODO: report error
        }
    } else {
        // aligned
        if (D_RODATA_START <= addr && addr < D_RODATA_START + D_RODATA_LENGTH) {
            // failure.
            // TODO: report error
        } else if (D_DATA_START <= addr && addr < D_DATA_START + D_DATA_LENGTH) {
            d_data[addr/4 - D_DATA_START] =
                (value & 0xffff) | (d_data[addr/4 - D_DATA_START] & 0xffff0000);
        } else if (D_BSS_START <= addr && addr < D_BSS_START + D_BSS_LENGTH) {
            d_bss[addr/4 - D_BSS_START] =
                (value & 0xffff) | (d_bss[addr/4 - D_BSS_START] & 0xffff0000);
        } else {
            // failure.
            // TODO: report error
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
            // failure.
            // TODO: report error
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
            // failure.
            // TODO: report error
        }
    }
}
