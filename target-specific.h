#pragma once
#include <stdint.h>

extern char so_md5_hash[33];

void        compute_hash_of_so(const char *so_name);
void        apply_target_specific_quirks(void);
int         target_specific_symbols(const char *symname, uint32_t func_addr);
