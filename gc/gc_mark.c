#include "gc_internal.h"

#include <stdint.h>

/* 
 * Mark all GC blocks reachable from the stack.
 * Scans conservatively from stack_bottom to a local variable.
 */
static void gc_mark_stack(t_gc_state *gc)
{
    uintptr_t start;
    uintptr_t end;
    uintptr_t p;
    int local;

    uintptr_t stack_local = (uintptr_t)&local;

    if (gc->stack_grows_up)
    {
        start = (uintptr_t)gc->stack_bottom;
        end   = stack_local;
    }
    else
    {
        start = stack_local;
        end   = (uintptr_t)gc->stack_bottom;
    }

    for (p = start; p + sizeof(void*) <= end; p += sizeof(void*))
    {
        uintptr_t word = *(uintptr_t*)p;
        t_gc_block *block = gc_find_block(gc, word);
        if (block && !block->marked)
            block->marked = 1;
    }
}

/* 
 * Mark GC blocks reachable from registered roots.
 * Users may still register roots for heap globals or long-lived pointers.
 */
static void gc_mark_registered_roots(t_gc_state *gc)
{
    t_gc_root *root;

    root = gc->roots;
    while (root)
    {
        t_gc_block *block = gc_find_block(gc, (uintptr_t)*(root->addr));
        if (block)
            block->marked = 1;
        root = root->next;
    }
}

/* 
 * Top-level marking function: marks all reachable blocks.
 * Scans the stack automatically and also scans registered roots.
 */
void gc_mark_from_roots(t_gc_state *gc)
{
    gc_mark_stack(gc);
    gc_mark_registered_roots(gc);
}
