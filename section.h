#pragma once

#include <stdint.h>
#include <stdio.h>
#include <bfd.h>

void        read_section(bfd *abfd, const char *name);
uint32_t    get_word_at(uint32_t addr);
char        get_char_at(uint32_t addr);
int         address_in_section(uint32_t addr, const char *name);
