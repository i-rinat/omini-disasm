#pragma once
#include <stdint.h>

void        process_instruction(uint32_t pc);
void        begin_function(uint32_t pc);
int         function_end_found(void);
void        end_function(void);
int         last_instruction_ended_with_return(void);
int         last_instruction_ended_with_goto(void);

// aux functions
uint32_t    arm_expand_imm12(uint32_t imm12);
uint32_t    arm_sign_extend_imm24(uint32_t imm24);
