#include "gc_internal.h"
#include <stdlib.h>
#include <string.h>

/* Register root first, then allocate */
void *gc_malloc(t_gc_state *gc, void **root_addr, size_t size)
{
	t_gc_block *block;

	if (size == 0)
		size = 1;

	if (root_addr)
		gc_add_root(gc, root_addr);

	block = gc_create_block(gc, size, 0);
	if (!block)
		return NULL;

	memset(block->payload, 0, size);

	if (gc->total_payload > gc->last_live + gc->next_threshold)
		gc_collect(gc);

	return block->payload;
}

void *gc_malloc_atomic(t_gc_state *gc, void **root_addr, size_t size)
{
	t_gc_block *block;

	if (size == 0)
		size = 1;

	if (root_addr)
		gc_add_root(gc, root_addr);

	block = gc_create_block(gc, size, 1);
	if (!block)
		return NULL;

	memset(block->payload, 0, size);

	if (gc->total_payload > gc->last_live + gc->next_threshold)
		gc_collect(gc);

	return block->payload;
}

/* Root-aware realloc */
void *gc_realloc(t_gc_state *gc, void **root_addr, void *ptr, size_t newsize)
{
	t_gc_block *found = gc->blocks;
	t_gc_block *prev = NULL;
	t_gc_block *nb;
	size_t copy;

	if (!ptr)
		return gc_malloc(gc, root_addr, newsize);

	while (found)
	{
		if (found->payload == ptr)
			break;
		prev = found;
		found = found->next;
	}

	if (!found)
		return realloc(ptr, newsize);

	nb = gc_create_block(gc, newsize, found->atomic);
	if (!nb)
		return NULL;

	copy = newsize < found->size ? newsize : found->size;
	memcpy(nb->payload, ptr, copy);
	if (newsize > copy)
		memset((char*)nb->payload + copy, 0, newsize - copy);

	gc_free_block(gc, prev, found);

	if (root_addr)
		*root_addr = nb->payload;

	return nb->payload;
}
