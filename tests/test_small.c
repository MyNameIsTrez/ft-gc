#include "gc.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    t_gc_state *gc = gc_create();
    if (!gc)
        return EXIT_FAILURE;

    int *p;
    int *q;

    printf("\033[36m[TEST_SMALL]\033[0m Starting small allocation test\n");

    /* Minimal example demonstrating automatic collection */
    for (int i = 0; i < 5; i++)
    {
        p = gc_malloc(gc, (void **)&p, sizeof(*p));
        q = gc_malloc_atomic(gc, (void **)&q, sizeof(*q));
        *p = i;
        *q = i * 2;

        printf("\033[34m[TEST_SMALL]\033[0m Iteration %d: allocated p=%p q=%p (p=%d, q=%d)\n",
               i, (void*)p, (void*)q, *p, *q);

        q = gc_realloc(gc, q, 2 * sizeof(*q));
        printf("\033[34m[TEST_SMALL]\033[0m Reallocated q=%p\n", (void*)q);
    }

    printf("\033[35m[TEST_SMALL]\033[0m Forcing GC...\n");
    gc_collect(gc);

    printf("\033[36m[TEST_SMALL]\033[0m Final stats:\n");
    gc_print_stats(gc);

    gc_destroy(gc);
    printf("\033[32m[TEST_SMALL]\033[0m Finished successfully\n");
    return EXIT_SUCCESS;
}
