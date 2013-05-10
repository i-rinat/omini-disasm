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

void        func_list_initialize();
void        func_list_add(uint32_t pc);
void        func_list_add_to_done_list(uint32_t pc);
void        func_list_mark_done(uint32_t pc);
uint32_t    func_list_get_done_count();
uint32_t    func_list_get_count();
uint32_t    func_list_get_next();
void        func_list_free();
uint32_t    func_list_pop_from_done_list();
