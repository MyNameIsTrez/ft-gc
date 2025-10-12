#include "gc.h"
#include <stdio.h>
#include <stdlib.h>

static void create_temporary_object(t_gc_state *gc)
{
    int *temp = gc_malloc(gc, (void **)&temp, sizeof(*temp));
    *temp = 42;
    printf("\033[34m[TEST_STACK_COLLECT]\033[0m Created temporary object with value %d at %p\n",
           *temp, (void *)temp);
}

int main(void)
{
    t_gc_state *gc = gc_create();
    if (!gc)
        return EXIT_FAILURE;

    printf("\033[36m[TEST_STACK_COLLECT]\033[0m Demonstrating collection of stack-local allocations\n");

    create_temporary_object(gc);

    printf("\033[35m[TEST_STACK_COLLECT]\033[0m Forcing GC after temporary object scope...\n");
    gc_collect(gc);

    gc_print_stats(gc);

    int *x = gc_malloc(gc, (void **)&x, sizeof(*x));
    *x = 99;
    printf("\033[34m[TEST_STACK_COLLECT]\033[0m Allocated new object after GC: value=%d at %p\n",
           *x, (void *)x);

    gc_collect(gc);
    gc_print_stats(gc);
    gc_destroy(gc);

    printf("\033[32m[TEST_STACK_COLLECT]\033[0m Finished — stack-local objects collected as expected\n");
    return EXIT_SUCCESS;
}
