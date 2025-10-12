#include "gc_internal.h"
#include <stdlib.h>

t_gc_block *gc_create_block(t_gc_state *gc, size_t size, int atomic)
{
    size_t hdr = sizeof(t_gc_block);
    hdr = (hdr + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    void *mem = malloc(hdr + size);
    if (!mem)
        return NULL;

    t_gc_block *block = (t_gc_block*)mem;
    block->next = gc->blocks;
    block->size = size;
    block->marked = 0;
    block->atomic = atomic ? 1 : 0;
    block->payload = (char*)mem + hdr;
    block->root = NULL;

    gc->blocks = block;
    gc->total_payload += size;
    return block;
}

void gc_free_block(t_gc_state *gc, t_gc_block *prev, t_gc_block *block)
{
    if (prev)
        prev->next = block->next;
    else
        gc->blocks = block->next;

    gc->total_payload -= block->size;
    free(block);
}

t_gc_block *gc_find_block(t_gc_state *gc, uintptr_t ptr)
{
    t_gc_block *block = gc->blocks;
    while (block)
    {
        uintptr_t start = (uintptr_t)block->payload;
        uintptr_t end = start + block->size;
        if (ptr >= start && ptr < end)
            return block;
        block = block->next;
    }
    return NULL;
}
