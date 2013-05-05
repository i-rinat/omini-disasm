#pragma once

#include <stdint.h>
#include <stdio.h>

uint32_t *
read_section(FILE *fp, uint32_t start, uint32_t length);

uint32_t
get_word_at(uint32_t addr);
