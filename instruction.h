#pragma once
#include <stdint.h>

void        process_instruction(uint32_t pc);
void        begin_function(uint32_t pc);
int         function_end_found();
void        end_function();
