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

    process_instruction(0x2170 + 0 * 4);
    process_instruction(0x2170 + 1 * 4);
    process_instruction(0x2170 + 2 * 4);
    process_instruction(0x2170 + 3 * 4);
    process_instruction(0x2170 + 4 * 4);
    process_instruction(0x2170 + 5 * 4);
    process_instruction(0x2170 + 6 * 4);
    process_instruction(0x2170 + 7 * 4);
    process_instruction(0x2170 + 8 * 4);
    process_instruction(0x2170 + 9 * 4);


    free(text);
    return 0;
}
