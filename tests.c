#include "gc.h"

#include <stdio.h>
#include <assert.h>

int main(void)
{
    volatile int dummy;
    t_gc_state *gc;
    int **p;
    int *q;
    int i;

    gc = gc_create((void *)&dummy);
    if (!gc)
    {
        fprintf(stderr, "Failed to create GC state\n");
        return 1;
    }

    for (i = 0; i < 100000; i++)
    {
        p = gc_malloc(gc, sizeof(*p));
        q = gc_malloc_atomic(gc, sizeof(*q));
        assert(*p == 0);
        // printf("i: %d\n", i); // TODO: REMOVE!
        *p = gc_realloc(gc, q, 2 * sizeof(*p));
    }

    printf("Heap size = %zu bytes\n", gc_get_heap_size(gc));
    gc_print_stats(gc);

    gc_collect(gc);
    printf("After manual collection:\n");
    gc_print_stats(gc);

    gc_destroy(gc);

    printf("Test passed!\n");
}
