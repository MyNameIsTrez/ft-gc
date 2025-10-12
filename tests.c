#include "gc.h"
#include <stdio.h>

int main(void)
{
	t_gc_state *gc = gc_create();
	if (!gc)
		return 1;

	int *p;
	int *q;

	for (int i = 0; i < 100000; i++)
	{
		p = gc_malloc(gc, (void **)&p, sizeof(*p));
		q = gc_malloc_atomic(gc, (void **)&q, sizeof(*q));
		*p = 0;
		*q = 0;

		/* realloc automatically updates root */
		gc_realloc(gc, (void **)&p, q, 2 * sizeof(*p));
	}

	gc_collect(gc);
	gc_destroy(gc);
	return 0;
}
