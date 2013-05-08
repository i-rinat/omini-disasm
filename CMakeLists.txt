project (omini-disasm)
cmake_minimum_required(VERSION 2.6)

add_definitions(-std=gnu99 -ggdb3 -Wall -Wextra)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SOMELIBS glib-2.0 REQUIRED)

link_directories(${SOMELIBS_LIBRARY_DIRS})
include_directories(${SOMELIBS_INCLUDE_DIRS})

add_executable (rec
    entry.c
    instruction.c
    section.c
    output.c
    pc-stack.c)

target_link_libraries(rec ${SOMELIBS_LIBRARIES})
