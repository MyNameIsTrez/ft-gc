#include "gc_internal.h"

void gc_mark_from_roots(t_gc_state *gc)
{
	t_gc_root *root = gc->roots;
	while (root)
	{
		void *ptr = *(root->addr);
		t_gc_block *block = gc_find_block(gc, (uintptr_t)ptr);
		if (block && !block->marked)
			block->marked = 1;
		root = root->next;
	}
}
