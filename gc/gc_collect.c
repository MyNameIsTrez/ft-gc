#include "gc_internal.h"

#include <stdio.h>

void gc_collect(t_gc_state *gc)
{
	gc_mark_from_roots(gc);
	gc_sweep(gc);

	gc->last_live = gc->total_payload;
	size_t next = gc->last_live * 2;
	if (next < GC_DEFAULT_THRESHOLD)
		next = GC_DEFAULT_THRESHOLD;
	gc->next_threshold = next;
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
	fprintf(stderr, "[minigc] blocks=%zu payload=%zu threshold=%zu\n",
			count, gc->total_payload, gc->next_threshold);
}
