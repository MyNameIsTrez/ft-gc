#include "gc_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void *gc_malloc_internal(t_gc_state *gc, void **root_addr, size_t size, int atomic)
{
    if (!root_addr)
    {
        fprintf(stderr, "\033[31m[GC ERROR]\033[0m root_addr must not be NULL.\n");
        return NULL;
    }

    if (size == 0)
        size = 1;

    t_gc_block *block = gc_create_block(gc, size, atomic);
    if (!block)
        return NULL;

    memset(block->payload, 0, size);
    gc_add_root(gc, root_addr, block);

    /* Automatic collection if threshold exceeded */
    if (gc->total_payload > gc->last_live + gc->next_threshold)
        gc_collect(gc);

    return block->payload;
}

void *gc_malloc(t_gc_state *gc, void **root_addr, size_t size)
{
    return gc_malloc_internal(gc, root_addr, size, 0);
}

void *gc_malloc_atomic(t_gc_state *gc, void **root_addr, size_t size)
{
    return gc_malloc_internal(gc, root_addr, size, 1);
}

void *gc_realloc(t_gc_state *gc, void *ptr, size_t newsize)
{
    if (!ptr)
    {
        fprintf(stderr, "\033[31m[GC ERROR]\033[0m cannot realloc a NULL pointer; use gc_malloc() with a valid root.\n");
        return NULL;
    }

    t_gc_block *old = gc_find_block(gc, (uintptr_t)ptr);
    if (!old)
    {
        fprintf(stderr, "\033[31m[GC ERROR]\033[0m cannot realloc non-GC pointer.\n");
        return NULL;
    }

    t_gc_block *nb = gc_create_block(gc, newsize, old->atomic);
    if (!nb)
        return NULL;

    size_t copy = newsize < old->size ? newsize : old->size;
    memcpy(nb->payload, ptr, copy);
    if (newsize > copy)
        memset((char*)nb->payload + copy, 0, newsize - copy);

    if (old->root)
    {
        *(old->root->addr) = nb->payload;
        nb->root = old->root;
        nb->root->block = nb;
    }

    t_gc_block *prev = NULL;
    t_gc_block *cur = gc->blocks;
    while (cur && cur != old)
    {
        prev = cur;
        cur = cur->next;
    }
    gc_free_block(gc, prev, old);

#ifdef GC_DEBUG
    printf("\033[33m[REALLOC]\033[0m old=%p new=%p size=%zu\n", (void*)old, (void*)nb, newsize);
#endif

    return nb->payload;
}
