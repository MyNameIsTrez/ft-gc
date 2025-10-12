#include "gc_internal.h"

void gc_mark_from_roots(t_gc_state *gc)
{
    t_gc_root *root = gc->roots;
    while (root)
    {
        if (root->block && !root->block->marked)
            root->block->marked = 1;
        root = root->next;
    }
}
