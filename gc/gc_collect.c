#include "gc_internal.h"
#include <stdio.h>

void gc_collect(t_gc_state *gc)
{
#ifdef GC_DEBUG
    printf("\033[35m[COLLECT START]\033[0m total_payload=%zu\n", gc->total_payload);
#endif

    gc_mark_from_roots(gc);
    gc_sweep(gc);

    gc->last_live = gc->total_payload;
    size_t next = gc->last_live * 2;
    if (next < GC_DEFAULT_THRESHOLD)
        next = GC_DEFAULT_THRESHOLD;
    gc->next_threshold = next;

#ifdef GC_DEBUG
    printf("\033[35m[COLLECT END]\033[0m total_payload=%zu next_threshold=%zu\n",
           gc->total_payload, gc->next_threshold);
#endif
}

size_t gc_get_heap_size(t_gc_state *gc)
{
    return gc->total_payload;
}

void gc_print_stats(t_gc_state *gc)
{
    size_t count = 0;
    t_gc_block *block = gc->blocks;
    while (block)
    {
        count++;
        block = block->next;
    }
    fprintf(stdout, "\033[36m[STATS]\033[0m blocks=%zu payload=%zu threshold=%zu\n",
            count, gc->total_payload, gc->next_threshold);
}
