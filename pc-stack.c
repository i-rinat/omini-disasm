#include "pc-stack.h"
#include <assert.h>
#include <stdlib.h>
#include <glib.h>


const uint32_t pc_stack_depth = 1000;

uint32_t *pc_stack = NULL;
uint32_t pc_stack_element_count = 0;

GHashTable *visited_bitmap;

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

void
visited_bitmap_initialize()
{
    visited_bitmap = g_hash_table_new(g_direct_hash, g_direct_equal);
    assert(visited_bitmap);
}

int
visited_bitmap_visited(uint32_t pc)
{
    return GPOINTER_TO_INT(g_hash_table_lookup(visited_bitmap, GINT_TO_POINTER(pc)));
}

void
visited_bitmap_mark_visited(uint32_t pc)
{
    g_hash_table_insert(visited_bitmap, GINT_TO_POINTER(pc), GINT_TO_POINTER(1));
}

void
visited_bitmap_free()
{
    g_hash_table_destroy(visited_bitmap);
}
