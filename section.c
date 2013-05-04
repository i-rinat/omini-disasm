#include "section.h"
#include <assert.h>
#include <stdlib.h>

uint32_t *
read_section(FILE *fp, uint32_t start, uint32_t length)
{
    uint32_t *buf = calloc(length, sizeof(uint32_t));
    assert(buf);
    fseek(fp, start, SEEK_SET);
    fread(buf, sizeof(uint32_t), length, fp);
    return buf;
}
