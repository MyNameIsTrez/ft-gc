#include "gc_internal.h"
#include <stdlib.h>
#include <stdint.h>

void gc_add_root(t_gc_state *gc, void **addr, t_gc_block *block)
{
    if (!gc || !addr)
        return;

    /* Determine whether addr is on the stack. We get the current stack
       address (stack_marker) and compare with the recorded stack_base.
       If addr falls within that range we assume it's a stack local and
       refuse to register a persistent root for it. */
    {
        volatile char stack_marker = 0;
        uintptr_t base = (uintptr_t)gc->stack_base;
        uintptr_t cur  = (uintptr_t)&stack_marker;
        uintptr_t a    = (uintptr_t)addr;
        int on_stack = 0;

        if (base == 0) {
            /* If stack_base wasn't initialized for some reason, be conservative:
               allow root registration. */
            on_stack = 0;
        } else if (base > cur) {
            /* typical: stack grows down, base above cur */
            if (a >= cur && a <= base)
                on_stack = 1;
        } else {
            /* stack grows up (rare) */
            if (a >= base && a <= cur)
                on_stack = 1;
        }

        if (on_stack) {
#ifdef GC_DEBUG
            printf("\033[36m[ROOT SKIP]\033[0m addr=%p looks like stack; not adding persistent root\n", (void*)addr);
#endif
            /* Do not register persistent root for stack-local addresses */
            return;
        }
    }

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
    printf("\033[36m[ROOT ADD]\033[0m root=%p block=%p addr=%p\n", (void*)root, (void*)block, (void*)addr);
#endif
}
