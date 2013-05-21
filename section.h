#pragma once

#include <stdint.h>
#include <stdio.h>
#include <bfd.h>

void        read_section(bfd *abfd, const char *name);
uint32_t    get_word_at(uint32_t addr);
