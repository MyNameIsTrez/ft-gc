#include "gc_internal.h"

#include <stdlib.h>

void gc_add_root(t_gc_state *gc, void **addr)
{
	t_gc_root *root = malloc(sizeof(t_gc_root));
	if (!root)
		return;

	root->addr = addr;
	root->next = gc->roots;
	gc->roots = root;
}
