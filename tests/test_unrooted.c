#include "gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void)
{
    printf("\033[36m[TEST_UNROOTED]\033[0m Starting unrooted block collection test\n");

    t_gc_state *gc = gc_create();
    if (!gc)
        return EXIT_FAILURE;

    // Allocate an unrooted "inner" object
    int *inner = gc_malloc(gc, NULL, sizeof(*inner));
    *inner = 1234;
    printf("\033[34m[TEST_UNROOTED]\033[0m Allocated inner object at %p with value %d\n",
           (void *)inner, *inner);

    // Force GC while inner is not referenced by any root
    printf("\033[35m[TEST_UNROOTED]\033[0m Forcing GC on unreferenced block...\n");
    gc_collect(gc);

    // Check whether the block still exists
    t_gc_block *found_inner = NULL;
    {
        extern t_gc_block *gc_find_block(t_gc_state *, uintptr_t);
        found_inner = gc_find_block(gc, (uintptr_t)inner);
    }

    if (!found_inner)
    {
        printf("\033[32m[PASS]\033[0m Unrooted inner block was correctly collected\n");
    }
    else
    {
        printf("\033[31m[FAIL]\033[0m Unrooted inner block unexpectedly survived\n");
        gc_destroy(gc);
        return EXIT_FAILURE;
    }

    gc_print_stats(gc);
    gc_destroy(gc);
    return EXIT_SUCCESS;
}
