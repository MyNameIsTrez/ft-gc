#include "gc.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_pair
{
    void *inner;
    int   value;
} t_pair;

int main(void)
{
    printf("\033[36m[TEST_TRANSITIVE]\033[0m Starting transitive marking test\n");

    t_gc_state *gc = gc_create();
    assert(gc);

    // Step 1: allocate the "inner" object
    int *inner = gc_malloc(gc, NULL, sizeof(*inner));
    *inner = 1234;
    printf("\033[34m[TEST_TRANSITIVE]\033[0m Allocated inner object at %p with value %d\n",
           (void *)inner, *inner);

    // Step 2: allocate the "outer" object and register it as a root
    t_pair *outer = gc_malloc(gc, (void **)&outer, sizeof(*outer));
    outer->inner = inner;
    outer->value = 42;
    printf("\033[34m[TEST_TRANSITIVE]\033[0m Allocated outer object at %p with value %d (outer->inner=%p)\n",
           (void *)outer, outer->value, (void *)outer->inner);

    // Step 3: force GC
    printf("\033[35m[TEST_TRANSITIVE]\033[0m Forcing GC...\n");
    gc_collect(gc);

    // Step 4: check if inner survived
    t_gc_block *found_inner = NULL;
    {
        extern t_gc_block *gc_find_block(t_gc_state *, uintptr_t);
        found_inner = gc_find_block(gc, (uintptr_t)inner);
    }

    if (found_inner)
    {
        printf("\033[32m[PASS]\033[0m Inner block survived — transitive GC works!\n");
    }
    else
    {
        printf("\033[31m[FAIL]\033[0m Inner block was freed — transitive GC failed!\n");
        gc_destroy(gc);
        return EXIT_FAILURE;
    }

    gc_print_stats(gc);
    gc_destroy(gc);
    return EXIT_SUCCESS;
}
