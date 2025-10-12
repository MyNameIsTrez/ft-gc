#include "gc.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("\033[36m[TEST_UNROOTED]\033[0m Starting NULL-root and realloc error tests\n");

    t_gc_state *gc = gc_create();
    if (!gc)
    {
        printf("\033[31m[FAIL]\033[0m Failed to create GC instance\n");
        return EXIT_FAILURE;
    }

    // Attempt allocations with NULL root — should fail
    int *p1 = gc_malloc(gc, NULL, sizeof(*p1));
    int *p2 = gc_malloc_atomic(gc, NULL, sizeof(*p2));

    if (!p1 && !p2)
    {
        printf("\033[32m[PASS]\033[0m gc_malloc() and gc_malloc_atomic() correctly rejected NULL root\n");
    }
    else
    {
        printf("\033[31m[FAIL]\033[0m gc_malloc() or gc_malloc_atomic() returned a pointer despite NULL root\n");
        if (p1) printf("\033[33m[WARN]\033[0m gc_malloc returned %p\n", (void*)p1);
        if (p2) printf("\033[33m[WARN]\033[0m gc_malloc_atomic returned %p\n", (void*)p2);
        gc_destroy(gc);
        return EXIT_FAILURE;
    }

    // Test realloc on NULL pointer — should trigger GC_ERROR
    void *r1 = gc_realloc(gc, NULL, 8);
    if (!r1)
    {
        printf("\033[32m[PASS]\033[0m gc_realloc correctly rejected NULL pointer with GC_ERROR\n");
    }
    else
    {
        printf("\033[31m[FAIL]\033[0m gc_realloc unexpectedly returned %p for NULL pointer\n", r1);
        free(r1);
        gc_destroy(gc);
        return EXIT_FAILURE;
    }

    // Test realloc on non-GC pointer — now expected to fail
    void *dummy = malloc(4);
    void *r2 = gc_realloc(gc, dummy, 8);
    if (!r2)
    {
        printf("\033[32m[PASS]\033[0m gc_realloc correctly rejected non-GC pointer with GC_ERROR\n");
    }
    else
    {
        printf("\033[31m[FAIL]\033[0m gc_realloc unexpectedly returned %p for non-GC pointer\n", r2);
        free(r2);
        gc_destroy(gc);
        return EXIT_FAILURE;
    }
    free(dummy);

    gc_destroy(gc);

    printf("\033[32m[TEST_UNROOTED]\033[0m Finished — all NULL-root and realloc error cases handled correctly\n");
    return EXIT_SUCCESS;
}
