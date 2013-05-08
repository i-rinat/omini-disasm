#pragma once
#include <stdint.h>


void        pc_stack_initialize();
int         pc_stack_empty();
uint32_t    pc_stack_pop();
void        pc_stack_push(uint32_t);
void        pc_stack_free();

void        visited_bitmap_initialize();
int         visited_bitmap_visited(uint32_t pc);
void        visited_bitmap_mark_visited(uint32_t pc);
void        visited_bitmap_free();
