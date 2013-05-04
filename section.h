#pragma once

#include <stdint.h>
#include <stdio.h>

uint32_t *
read_section(FILE *fp, uint32_t start, uint32_t length);
