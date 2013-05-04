#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "section.h"

int main(void)
{
    printf("hello, world!\n");

    FILE *fp = fopen("libplasma.so", "wb");
    uint32_t *text = read_section(fp, 0x12e8, 0x2df0);
    fclose(fp);



    free(text);
    return 0;
}
