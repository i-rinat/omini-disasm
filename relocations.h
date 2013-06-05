#pragma once
#include <bfd.h>

void        process_relocations(bfd *abfd, asymbol **symbol_table);
