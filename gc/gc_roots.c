#include "gc_internal.h"
#include <stdlib.h>

void gc_add_root(t_gc_state *gc, void **addr, t_gc_block *block)
{
    t_gc_root *root = malloc(sizeof(t_gc_root));
    if (!root)
        return;

    root->addr = addr;
    root->block = block;
    root->next = gc->roots;
    gc->roots = root;

    if (block)
        block->root = root;

#ifdef GC_DEBUG
    printf("[minigc] ROOT ADD root=%p block=%p addr=%p\n", (void*)root, (void*)block, (void*)addr);
#endif
}
