#include "gc_internal.h"
#include <stdlib.h>
#include <string.h> /* for memcpy */
#include <stdint.h>

/* Simple stack to hold blocks to visit during mark. */
static void mark_push(t_gc_block ***stackp, size_t *cap, size_t *len, t_gc_block *b)
{
    if (*len >= *cap)
    {
        size_t newcap = (*cap == 0) ? 16 : (*cap * 2);
        t_gc_block **newstack = (t_gc_block **)realloc(*stackp, newcap * sizeof(t_gc_block *));
        if (!newstack)
            return; /* allocation failure -> best-effort: drop marking further */
        *stackp = newstack;
        *cap = newcap;
    }
    (*stackp)[(*len)++] = b;
}

static t_gc_block *mark_pop(t_gc_block **stack, size_t *len)
{
    if (*len == 0) return NULL;
    return stack[--(*len)];
}

void gc_mark_from_roots(t_gc_state *gc)
{
    if (!gc) return;

    t_gc_root *root = gc->roots;

    /* Dynamic stack for blocks to visit */
    t_gc_block **stack = NULL;
    size_t cap = 0;
    size_t len = 0;

    /* Initialize: push all root->block that exist and mark them */
    while (root)
    {
        if (root->block && !root->block->marked)
        {
            root->block->marked = 1;
            mark_push(&stack, &cap, &len, root->block);
        }
        root = root->next;
    }

    /* Visit blocks in stack (DFS) */
    while (len > 0)
    {
        t_gc_block *blk = mark_pop(stack, &len);
        if (!blk) break;

        /* If block is atomic, do not scan its payload for pointers */
        if (blk->atomic)
            continue;

        /* Conservative scan: examine the payload as a sequence of uintptr_t-sized words. */
        uintptr_t start = (uintptr_t)blk->payload;
        uintptr_t end = start + blk->size;
        uintptr_t cursor = start;

        while (cursor + sizeof(uintptr_t) <= end)
        {
            uintptr_t candidate = 0;
            memcpy(&candidate, (void *)cursor, sizeof(candidate));

            if (candidate != 0)
            {
                t_gc_block *target = gc_find_block(gc, candidate);
                if (target && !target->marked)
                {
                    target->marked = 1;
                    mark_push(&stack, &cap, &len, target);
                }
            }

            cursor += sizeof(uintptr_t);
        }
    }

    free(stack);
}
