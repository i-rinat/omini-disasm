#pragma once
#include <stdint.h>


void        pc_stack_initialize(void);
int         pc_stack_empty(void);
uint32_t    pc_stack_pop(void);
void        pc_stack_push(uint32_t);
void        pc_stack_free(void);

void        visited_bitmap_initialize(void);
int         visited_bitmap_visited(uint32_t pc);
void        visited_bitmap_mark_visited(uint32_t pc);
void        visited_bitmap_free(void);

void        codecoverage_bitmap_initialize(void);
int         codecoverage_bitmap_visited(uint32_t pc);
void        codecoverage_bitmap_mark_visited(uint32_t pc);
void        codecoverage_bitmap_free(void);

void        func_list_initialize(void);
void        func_list_add(uint32_t pc);
void        func_list_add_to_ignore_list(uint32_t pc);
void        func_list_mark_done(uint32_t pc);
uint32_t    func_list_get_done_count(void);
uint32_t    func_list_get_count(void);
uint32_t    func_list_get_next(void);
void        func_list_free(void);
uint32_t    func_list_pop_from_done_list(void);
int         func_list_already_in_done_list(uint32_t pc);
void        func_list_mark_as_non_returning(uint32_t pc);
int         func_list_is_non_returning(uint32_t pc);
