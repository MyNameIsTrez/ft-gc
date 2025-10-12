#include "gc.h"

#include <stdio.h>

int main(void)
{
    t_gc_state *gc = gc_create();
    if (!gc)
        return 1;

    int *p;
    int *q;

    /* Stress test: allocate a huge number of small blocks */
    for (int i = 0; i < 100000; i++)
    {
        p = gc_malloc(gc, (void **)&p, sizeof(*p));
        q = gc_malloc_atomic(gc, (void **)&q, sizeof(*q));
        *p = i;
        *q = i;

        /* Realloc q periodically */
        q = gc_realloc(gc, q, 2 * sizeof(*q));

        /* Print progress every 10,000 iterations */
        if (i % 10000 == 0)
            printf("\033[34m[TEST_LARGE]\033[0m Iteration %d, heap size: %zu\n", i, gc_get_heap_size(gc));
    }

    /* Final collection and stats */
    gc_collect(gc);

    printf("\033[36m[TEST_LARGE]\033[0m Final stats:\n");
    gc_print_stats(gc);

    gc_destroy(gc);
    return 0;
}
