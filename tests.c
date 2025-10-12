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
        printf("i: %d\n", i);
        p = gc_malloc(gc, (void **)&p, sizeof(*p));
        q = gc_malloc_atomic(gc, (void **)&q, sizeof(*q));
        *p = 0;
        *q = 0;

        q = gc_realloc(gc, q, 2 * sizeof(*q));
    }

    gc_collect(gc);
    gc_destroy(gc);
    return 0;
}
