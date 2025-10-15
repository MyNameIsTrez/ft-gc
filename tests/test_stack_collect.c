#include "gc.h"
#include "gc_internal.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Helper function to create a temporary stack-local object */
static void create_temporary_object(t_gc_state *gc, uintptr_t *out_addr)
{
    // TODO: Add back?
    // Force temp to be far ahead in the stack
    // volatile char padding[1024 * 1024]; // 1 MB stack padding
    // for (size_t i = 0; i < sizeof(padding); i++)
    //     padding[i] = (char)i; // prevent compiler optimization

    int *temp = gc_malloc(gc, (void **)&temp, sizeof(*temp)); // rsp=0x7fffffffd6b0
    *temp = 42;
    printf("\033[34m[TEST_STACK_COLLECT]\033[0m Created temporary object with value %d at %p\n",
           *temp, (void *)temp);

    // Return the address of the allocation for later checking
    if (out_addr)
        *out_addr = (uintptr_t)temp;

    // Force GC immediately to ensure it's still tracked
    gc_collect(gc); // rsp=0x7fffffffd6b0

    // Check that it's still alive immediately after allocation
    t_gc_block *blk = gc_find_block(gc, (uintptr_t)temp);
    if (!blk)
    {
        printf("\033[31m[FAIL]\033[0m Temporary object was collected unexpectedly\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("\033[32m[PASS]\033[0m Temporary object still tracked by GC\n");
    }
}

int main(void)
{
    // rsp=0x7fffffffd780
    t_gc_state *gc = gc_create();
    if (!gc)
    {
        printf("\033[31m[FAIL]\033[0m Failed to create GC instance\n");
        return EXIT_FAILURE;
    }

    printf("\033[36m[TEST_STACK_COLLECT]\033[0m Demonstrating collection of stack-local allocations\n"); // rsp=0x7fffffffd780

    uintptr_t temp_addr = 0; // rsp=0x7fffffffd780
    create_temporary_object(gc, &temp_addr);

    printf("\033[35m[TEST_STACK_COLLECT]\033[0m Forcing GC after temporary object scope in main...\n");
    gc_collect(gc);

    // Check that the temporary object was collected (since it is out of scope)
    t_gc_block *blk = gc_find_block(gc, temp_addr);
    if (blk)
    {
        printf("\033[31m[FAIL]\033[0m Temporary object still exists after going out of scope\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("\033[32m[PASS]\033[0m Temporary object correctly collected after going out of scope\n");
    }

    // Allocate a new persistent object
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
