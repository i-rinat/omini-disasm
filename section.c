#include "section.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct section_t {
    uint32_t   *buf;
    uint32_t    start;
    uint32_t    length;
    char       *name;
    struct section_t *next;
} section_t;

section_t *sections = NULL;

static
void
append_section(uint32_t *buf, uint32_t start, uint32_t length, const char *name)
{
    section_t **ptr = &sections;
    while (*ptr != NULL)
        ptr = &((*ptr)->next);

    *ptr = malloc(sizeof(section_t));
    assert(*ptr);

    void *internal_buf = malloc(length);
    assert(internal_buf);
    memcpy(internal_buf, buf, length);

    (*ptr)->buf = internal_buf;
    (*ptr)->start = start;
    (*ptr)->length = length;
    (*ptr)->name = strdup(name);
    (*ptr)->next = NULL;
}

void
read_section(bfd *abfd, const char *name)
{
    struct bfd_section *asect = abfd->sections;
    FILE *fp = fopen(abfd->filename, "rb");
    assert(fp);

    while (asect) {
        if (!strcmp(asect->name, name)) {
            fseek(fp, asect->filepos, SEEK_SET);
            void *buf = malloc(asect->size);
            assert(buf);
            uint read_bytes = fread(buf, 1, asect->size, fp);
            assert(read_bytes == asect->size);
            append_section(buf, asect->vma, asect->size, name);
            printf("loaded section %s of size %x at %x\n", name,
                    (uint32_t)asect->size, (uint32_t)asect->vma);
            free(buf);
        }
        asect = asect->next;
    }

    fclose(fp);
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

int
address_in_section(uint32_t addr, const char *name)
{
    assert((addr & 0x3) == 0 && "address alignment");
    section_t *ptr = sections;
    while (NULL != ptr) {
        if (ptr->start <= addr && addr < ptr->start + ptr->length)
            if (!strcmp(ptr->name, name))
                return 1;
        ptr = ptr->next;
    }
    return 0;
}
