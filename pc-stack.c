#include "pc-stack.h"
#include <assert.h>
#include <stdlib.h>
#include <glib.h>


const uint32_t pc_stack_depth = 1000;

uint32_t *pc_stack = NULL;
uint32_t pc_stack_element_count = 0;

GHashTable *visited_bitmap;
GHashTable *func_list;
GHashTable *func_list_do_not_touch;
GHashTable *func_list_actually_done;


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

void
func_list_initialize()
{
    func_list = g_hash_table_new(g_direct_hash, g_direct_equal);
    func_list_do_not_touch = g_hash_table_new(g_direct_hash, g_direct_equal);
    func_list_actually_done = g_hash_table_new(g_direct_hash, g_direct_equal);
}

void
func_list_add(uint32_t pc)
{
    gpointer already_in_1 = g_hash_table_lookup(func_list, GINT_TO_POINTER(pc));
    gpointer already_in_2 = g_hash_table_lookup(func_list_do_not_touch, GINT_TO_POINTER(pc));

    if (! (already_in_1 || already_in_2)) {
        g_hash_table_insert(func_list, GINT_TO_POINTER(pc), GINT_TO_POINTER(1));
    }
}

void
func_list_add_to_ignore_list(uint32_t pc)
{
    gpointer already_in_1 = g_hash_table_lookup(func_list, GINT_TO_POINTER(pc));
    gpointer already_in_2 = g_hash_table_lookup(func_list_do_not_touch, GINT_TO_POINTER(pc));

    if (! (already_in_1 || already_in_2)) {
        g_hash_table_insert(func_list_do_not_touch, GINT_TO_POINTER(pc), GINT_TO_POINTER(1));
    }
}

void
func_list_mark_done(uint32_t pc)
{
    gpointer already_in_1 = g_hash_table_lookup(func_list, GINT_TO_POINTER(pc));

    if (already_in_1) {
        g_hash_table_remove(func_list, GINT_TO_POINTER(pc));
        g_hash_table_insert(func_list_do_not_touch, GINT_TO_POINTER(pc), GINT_TO_POINTER(1));
        g_hash_table_insert(func_list_actually_done, GINT_TO_POINTER(pc), GINT_TO_POINTER(1));
    }

}

uint32_t
func_list_get_next()
{
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, func_list);
    if (g_hash_table_iter_next(&iter, &key, &value)) {
        return GPOINTER_TO_INT(key);
    }
    assert(0 && "requested func addr from func_list while there is no funcs in list");
    return 0;
}

uint32_t
func_list_pop_from_done_list()
{
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, func_list_actually_done);
    if (g_hash_table_iter_next(&iter, &key, &value)) {
        g_hash_table_iter_remove(&iter);
        return GPOINTER_TO_INT(key);
    }

    return 0;
}

int
func_list_already_in_done_list(uint32_t pc)
{
    gpointer already_in_2 = g_hash_table_lookup(func_list_do_not_touch, GINT_TO_POINTER(pc));
    if (already_in_2)
        return 1;
    return 0;
}

void
func_list_free()
{
    g_hash_table_destroy(func_list);
    g_hash_table_destroy(func_list_do_not_touch);
    g_hash_table_destroy(func_list_actually_done);
}

uint32_t
func_list_get_done_count()
{
    return g_hash_table_size(func_list_actually_done);
}

uint32_t
func_list_get_count()
{
    return g_hash_table_size(func_list);
}
