#include "gc_internal.h"
#include <stdlib.h>

t_gc_state *gc_create(void)
{
    t_gc_state *gc = calloc(1, sizeof(t_gc_state));
    if (!gc)
        return NULL;
    gc->next_threshold = GC_DEFAULT_THRESHOLD;
    return gc;
}

void gc_destroy(t_gc_state *gc)
{
    t_gc_block *block = gc->blocks;
    while (block)
    {
        t_gc_block *next = block->next;
        free(block);
        block = next;
    }

    t_gc_root *root = gc->roots;
    while (root)
    {
        t_gc_root *next = root->next;
        free(root);
        root = next;
    }

    free(gc);
}
