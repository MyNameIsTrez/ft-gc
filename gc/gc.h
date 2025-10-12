#pragma once
#include <stddef.h>

/**
 * @file gc.h
 * @brief A simple mark-and-sweep garbage collector for C.
 *
 * Provides managed heap allocations with optional persistent roots,
 * automatic collection of stack-local objects, and support for
 * atomic (non-pointer) blocks.
 */

#define GC_DEFAULT_THRESHOLD 1048576  /**< Default GC growth threshold in bytes (1 MiB) */

/**
 * @struct s_gc_block
 * @brief Metadata for a single allocated block in the GC-managed heap.
 *
 * Each block tracks its size, marked status, atomic flag, payload pointer,
 * and a link to its root for fast updates during reallocations.
 */
typedef struct s_gc_block
{
    struct s_gc_block *next;   /**< Next block in the heap linked list */
    size_t size;               /**< Payload size in bytes */
    unsigned char marked;      /**< Mark flag used during GC traversal */
    unsigned char atomic;      /**< True if block contains no pointers */
    void *payload;             /**< Pointer to the actual memory allocated */
    struct s_gc_root *root;    /**< Link to its root for O(1) root updates */
} t_gc_block;

/**
 * @struct s_gc_root
 * @brief Tracks a root pointer for GC marking.
 *
 * Roots point to blocks that should not be collected.
 * Non-rooted stack allocations can still exist but are collectible when out of scope.
 */
typedef struct s_gc_root
{
    struct s_gc_root *next; /**< Next root in the linked list */
    void **addr;            /**< Address of the user pointer pointing to the block */
    t_gc_block *block;      /**< Direct link to the block for O(1) marking */
} t_gc_root;

/**
 * @struct s_gc_state
 * @brief Global state of the garbage collector.
 *
 * Tracks all allocated blocks, roots, total heap usage,
 * thresholds for triggering collection, and an approximate stack base.
 */
typedef struct s_gc_state
{
    t_gc_block *blocks;       /**< Head of linked list of all blocks */
    t_gc_root *roots;         /**< Head of linked list of registered roots */
    size_t total_payload;     /**< Current total payload size in bytes */
    size_t last_live;         /**< Payload size of last GC collection */
    size_t next_threshold;    /**< Heap size threshold to trigger next GC */
    void *stack_base;         /**< Approximate stack base for detecting stack-local pointers */
} t_gc_state;


/*----------------------------------*/
/*      Garbage Collector API       */
/*----------------------------------*/

/**
 * @brief Initialize a new garbage collector instance.
 * @return Pointer to a newly allocated t_gc_state, or NULL on failure.
 *
 * Must be destroyed with gc_destroy() to free all resources.
 */
t_gc_state *gc_create(void);

/**
 * @brief Destroy a garbage collector instance and free all managed memory.
 * @param gc Pointer to GC state.
 *
 * Frees all heap blocks and roots. After calling, the gc pointer
 * must not be used.
 */
void gc_destroy(t_gc_state *gc);

/**
 * @brief Allocate a new managed block.
 * @param gc GC state pointer.
 * @param root_addr Optional address of a pointer to register as a root.
 *                  Pass NULL if this allocation is temporary.
 * @param size Number of bytes to allocate.
 * @return Pointer to allocated memory (payload) or NULL on failure.
 *
 * Non-atomic allocations are scanned for pointers during GC.
 */
void *gc_malloc(t_gc_state *gc, void **root_addr, size_t size);

/**
 * @brief Allocate a new atomic block (contains no pointers).
 * @param gc GC state pointer.
 * @param root_addr Optional address of a pointer to register as a root.
 *                  Pass NULL if temporary.
 * @param size Number of bytes to allocate.
 * @return Pointer to allocated memory or NULL on failure.
 *
 * Atomic blocks are not scanned for pointers during marking.
 */
void *gc_malloc_atomic(t_gc_state *gc, void **root_addr, size_t size);

/**
 * @brief Reallocate a GC-managed block to a new size.
 * @param gc GC state pointer.
 * @param ptr Pointer to an existing GC-managed block payload.
 * @param newsize New size in bytes.
 * @return Pointer to reallocated memory payload or NULL on failure.
 *
 * If the block has a root, the root pointer is updated automatically.
 * The old block is freed after the copy.
 */
void *gc_realloc(t_gc_state *gc, void *ptr, size_t newsize);

/**
 * @brief Perform garbage collection.
 * @param gc GC state pointer.
 *
 * Marks all reachable blocks starting from roots and frees
 * unmarked blocks. Updates GC statistics and thresholds.
 */
void gc_collect(t_gc_state *gc);

/**
 * @brief Get the total heap size currently used by GC-managed blocks.
 * @param gc GC state pointer.
 * @return Total payload size in bytes.
 */
size_t gc_get_heap_size(t_gc_state *gc);

/**
 * @brief Print GC statistics to stdout.
 * @param gc GC state pointer.
 *
 * Includes number of blocks, total payload, and collection thresholds.
 */
void gc_print_stats(t_gc_state *gc);
