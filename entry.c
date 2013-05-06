#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "section.h"

int main(void)
{
    printf("rec started\n");

    FILE *fp = fopen("libplasma.so", "rb");
    uint32_t *text = read_section(fp, 0x12e8, 0x2df0);
    fclose(fp);

    for (int k = 0; k < 16; k ++)
        process_instruction(0x2170 + k * 4);

    free(text);
    return 0;
}
