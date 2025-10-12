#include "gc_internal.h"

#include <stdlib.h>

/* Create a new GC state with a captured stack bottom */
#include "gc_internal.h"
#include <stdlib.h>

t_gc_state *gc_create(void *stack_bottom)
{
    t_gc_state *gc;
    int local;

    gc = (t_gc_state *)calloc(1, sizeof(t_gc_state));
    if (!gc)
        return NULL;

    gc->stack_bottom = stack_bottom;
    gc->stack_grows_up = ((uintptr_t)&local) > (uintptr_t)stack_bottom ? 1 : 0;
    gc->next_threshold = GC_DEFAULT_THRESHOLD;
    gc->total_payload = 0;
    gc->last_live = 0;

    return gc;
}


/* Destroy the GC state and free all blocks and roots */
void gc_destroy(t_gc_state *gc)
{
    t_gc_block *block;
    t_gc_block *next_block;
    t_gc_root *root;
    t_gc_root *next_root;

    block = gc->blocks;
    while (block)
    {
        next_block = block->next;
        free(block);
        block = next_block;
    }

    root = gc->roots;
    while (root)
    {
        next_root = root->next;
        free(root);
        root = next_root;
    }

    free(gc);
}
