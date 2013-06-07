#pragma once
#include <bfd.h>
#include <stdint.h>

extern uint32_t setjmp_func_address;
extern uint32_t setjmp_plt_func_address;

void        process_relocations(bfd *abfd, asymbol **symbol_table);
