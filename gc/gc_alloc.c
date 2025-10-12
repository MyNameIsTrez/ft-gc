#include "gc_internal.h"

#include <stdlib.h>
#include <string.h>

void *gc_malloc(t_gc_state *gc, size_t size)
{
    t_gc_block *block;

    if (size == 0)
        size = 1;

    block = gc_create_block(gc, size, 0);
    if (!block)
        return NULL;

    memset(block->payload, 0, size);

    if (gc->total_payload > gc->last_live + gc->next_threshold)
        gc_collect(gc);

    return block->payload;
}

void *gc_malloc_atomic(t_gc_state *gc, size_t size)
{
    t_gc_block *block;

    if (size == 0)
        size = 1;

    block = gc_create_block(gc, size, 1);
    if (!block)
        return NULL;

    memset(block->payload, 0, size);

    if (gc->total_payload > gc->last_live + gc->next_threshold)
        gc_collect(gc);

    return block->payload;
}

static t_gc_block *gc_find_prev_block(t_gc_state *gc, void *ptr)
{
    t_gc_block *block;
    t_gc_block *prev;

    block = gc->blocks;
    prev = NULL;
    while (block)
    {
        if (block->payload == ptr)
            return prev;
        prev = block;
        block = block->next;
    }
    return NULL;
}

void *gc_realloc(t_gc_state *gc, void *ptr, size_t newsize)
{
    t_gc_block *found;
    t_gc_block *prev;
    t_gc_block *nb;
    size_t copy;

    if (!ptr)
        return gc_malloc(gc, newsize);

    found = gc->blocks;
    prev = gc_find_prev_block(gc, ptr);

    while (found)
    {
        if (found->payload == ptr)
            break;
        found = found->next;
    }

    if (!found)
        return realloc(ptr, newsize);

    nb = gc_create_block(gc, newsize, found->atomic);
    if (!nb)
        return NULL;

    copy = newsize < found->size ? newsize : found->size;
    memcpy(nb->payload, ptr, copy);
    if (newsize > copy)
        memset((char *)nb->payload + copy, 0, newsize - copy);

    gc_free_block(gc, prev, found);

    if (gc->total_payload > gc->last_live + gc->next_threshold)
        gc_collect(gc);

    return nb->payload;
}
