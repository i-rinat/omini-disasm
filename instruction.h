#pragma once
#include <stdint.h>

void        process_instruction(uint32_t pc);
void        begin_function(uint32_t pc);
int         inside_function();
void        end_function();
