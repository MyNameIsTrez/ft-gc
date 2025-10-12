#include "gc.h"

#include <stdio.h>

static void create_temporary_object(t_gc_state *gc)
{
    int *temp = gc_malloc(gc, (void **)&temp, sizeof(*temp));
    *temp = 42;
    printf("\033[34m[TEST_STACK_COLLECT]\033[0m Created temporary object with value %d at %p\n",
           *temp, (void *)temp);

    /* temp goes out of scope after this function returns */
}

int main(void)
{
    t_gc_state *gc = gc_create();
    if (!gc)
        return 1;

    printf("\033[36m[TEST_STACK_COLLECT]\033[0m Demonstrating collection of stack-local allocations\n");

    /* Step 1: Create a scoped allocation */
    create_temporary_object(gc);

    /* Step 2: Force garbage collection immediately */
    printf("\033[35m[TEST_STACK_COLLECT]\033[0m Forcing collection after temp has gone out of scope...\n");
    gc_collect(gc);

    /* Step 3: Show stats after collection */
    gc_print_stats(gc);

    /* Step 4: Allocate something new to trigger further GC activity */
    int *x = gc_malloc(gc, (void **)&x, sizeof(*x));
    *x = 99;
    printf("\033[34m[TEST_STACK_COLLECT]\033[0m Allocated new object after collection, value=%d at %p\n",
           *x, (void *)x);

    gc_collect(gc);
    gc_print_stats(gc);
    gc_destroy(gc);

    printf("\033[32m[TEST_STACK_COLLECT]\033[0m Finished — notice how the temporary object was collected early.\n");
    return 0;
}
