#include "gc.h"

#include <stdio.h>

int main(void)
{
    t_gc_state *gc = gc_create();
    if (!gc)
        return 1;

    int *p;
    int *q;

    /* Minimal example demonstrating automatic collection */
    for (int i = 0; i < 5; i++)
    {
        p = gc_malloc(gc, (void **)&p, sizeof(*p));
        q = gc_malloc_atomic(gc, (void **)&q, sizeof(*q));
        *p = i;
        *q = i * 2;

        /* Realloc q to demonstrate automatic root update */
        q = gc_realloc(gc, q, 2 * sizeof(*q));

#ifdef GC_DEBUG
        printf("\033[34m[TEST_SMALL]\033[0m Iteration %d, heap size: %zu\n", i, gc_get_heap_size(gc));
#endif
    }

    /* Collect and print final stats */
    gc_collect(gc);

    printf("\033[36m[TEST_SMALL]\033[0m Final stats:\n");
    gc_print_stats(gc);

    gc_destroy(gc);
    return 0;
}
