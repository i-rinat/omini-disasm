#include "pc-stack.h"
#include <assert.h>
#include <stdlib.h>


const uint32_t pc_stack_depth = 1000;

uint32_t *pc_stack = NULL;
uint32_t pc_stack_element_count = 0;

void
pc_stack_initialize()
{
    pc_stack = malloc(sizeof(uint32_t) * pc_stack_depth);
    assert(pc_stack);
}

int
pc_stack_empty()
{
    return (pc_stack_element_count == 0);
}

uint32_t
pc_stack_pop()
{
    assert(pc_stack_element_count > 0);
    pc_stack_element_count --;
    return pc_stack[pc_stack_element_count];
}

void
pc_stack_push(uint32_t e)
{
    assert(pc_stack_element_count < pc_stack_depth);
    pc_stack[pc_stack_element_count] = e;
    pc_stack_element_count ++;
}

void
pc_stack_free()
{
    free(pc_stack);
    pc_stack = NULL;
}
