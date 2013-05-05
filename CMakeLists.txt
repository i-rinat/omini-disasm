project (omini-disasm)
cmake_minimum_required(VERSION 2.6)

add_definitions(-std=gnu99 -ggdb3 -Wall -Wextra)

add_executable (rec
    entry.c
    instruction.c
    section.c
    output.c)
