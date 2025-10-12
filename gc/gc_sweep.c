#include "gc_internal.h"

void gc_sweep(t_gc_state *gc)
{
	t_gc_block *prev = NULL;
	t_gc_block *block = gc->blocks;

	while (block)
	{
		if (block->marked)
		{
			block->marked = 0;
			prev = block;
			block = block->next;
		}
		else
		{
			t_gc_block *cur = block;
			block = block->next;
			gc_free_block(gc, prev, cur);
		}
	}
}
