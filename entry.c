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

    uint32_t ip = (0x2170 - 0x12e8)/4;
    process_instruction(text[ip]);
    process_instruction(text[ip + 1]);
    process_instruction(text[ip + 2]);
    process_instruction(text[ip + 3]);
    process_instruction(text[ip + 4]);
    process_instruction(text[ip + 5]);
    process_instruction(text[ip + 6]);

    free(text);
    return 0;
}
