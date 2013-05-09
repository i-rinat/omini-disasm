#include "section.h"
#include <assert.h>
#include <stdlib.h>

typedef struct section_t {
    uint32_t   *buf;
    uint32_t    start;
    uint32_t    length;
    struct section_t *next;
} section_t;

section_t *sections = NULL;

static
void
append_section(uint32_t *buf, uint32_t start, uint32_t length)
{
    section_t **ptr = &sections;
    while (*ptr != NULL)
        ptr = &((*ptr)->next);

    *ptr = malloc(sizeof(section_t));
    assert(*ptr);

    (*ptr)->buf = buf;
    (*ptr)->start = start;
    (*ptr)->length = length;
    (*ptr)->next = NULL;
}

uint32_t *
read_section(FILE *fp, uint32_t start, uint32_t length)
{
    uint32_t *buf = calloc(length, sizeof(uint32_t));
    assert(buf);
    fseek(fp, start, SEEK_SET);
    fread(buf, sizeof(uint32_t), length, fp);
    append_section(buf, start, length);
    return buf;
}

uint32_t
get_word_at(uint32_t addr)
{
    assert((addr & 0x3) == 0 && "address alignment");
    section_t *ptr = sections;
    while (NULL != ptr) {
        if (ptr->start <= addr && addr < ptr->start + ptr->length)
            return ptr->buf[(addr - ptr->start) / 4];

        ptr = ptr->next;
    }
    printf("get_word_at(0x%04x)\n", addr);
    assert(0 && "can't get word at address");
    return 0;
}
