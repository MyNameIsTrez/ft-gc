#include "gc_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Stack helpers for DFS marking */
static void mark_push(t_gc_block ***stackp, size_t *cap, size_t *len, t_gc_block *b)
{
    if (*len >= *cap)
    {
        size_t newcap = (*cap == 0) ? 16 : (*cap * 2);
        t_gc_block **newstack = (t_gc_block **)realloc(*stackp, newcap * sizeof(t_gc_block *));
        if (!newstack)
            return;
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

    volatile char stack_marker;
    uintptr_t cur_sp = (uintptr_t)&stack_marker; // rsp=0x7fffffffd4e0
    printf("cur_sp:         %p\n", (void *)cur_sp); // prints 0x7bfff4700030 TODO: REMOVE!

    uintptr_t stack_low, stack_high;
    if ((uintptr_t)gc->stack_base > cur_sp) // p gc->stack_base prints 0x7bfff4500020
    {
        stack_high = (uintptr_t)gc->stack_base;
        stack_low  = cur_sp;
    }
    else
    {
        stack_high = cur_sp;
        stack_low  = (uintptr_t)gc->stack_base;
    }

    printf("\033[33m[GC DEBUG]\033[0m stack_low=%p stack_high=%p cur_sp=%p\n",
           (void*)stack_low, (void*)stack_high, (void*)cur_sp);

    t_gc_root *root = gc->roots;
    t_gc_block **stack = NULL;
    size_t cap = 0, len = 0;

    while (root)
    {
        if (!root->block)
        {
            root = root->next;
            continue;
        }

        uintptr_t addr = (uintptr_t)root->addr;

        // Debug print: root address and block payload
        printf("\033[33m[GC DEBUG]\033[0m root=%p addr=%p block_payload=%p size=%zu\n",
               (void*)root, (void*)addr, root->block->payload, root->block->size);

        int live = (addr >= stack_low && addr <= stack_high);
        printf("\033[33m[GC DEBUG]\033[0m addr %p is %sactive\n",
               (void*)addr, live ? "" : "NOT ");

        if (live && !root->block->marked)
        {
            root->block->marked = 1;
            mark_push(&stack, &cap, &len, root->block);
        }

        root = root->next;
    }

    /* DFS marking through payloads */
    while (len > 0)
    {
        t_gc_block *blk = mark_pop(stack, &len);
        if (!blk) break;

        if (blk->atomic) continue;

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
