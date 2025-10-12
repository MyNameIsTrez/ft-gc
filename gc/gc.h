#pragma once

#include <stddef.h>

#define GC_DEFAULT_THRESHOLD 1048576  // 1 MiB

typedef struct s_gc_block
{
	struct s_gc_block *next;
	size_t size;
	unsigned char marked;
	unsigned char atomic;
	void *payload;
} t_gc_block;

typedef struct s_gc_root
{
	struct s_gc_root *next;
	void **addr;
} t_gc_root;

typedef struct s_gc_state
{
	t_gc_block *blocks;
	t_gc_root *roots;
	size_t total_payload;
	size_t last_live;
	size_t next_threshold;
} t_gc_state;

t_gc_state *gc_create(void);
void         gc_destroy(t_gc_state *gc);

void       *gc_malloc(t_gc_state *gc, void **root_addr, size_t size);
void       *gc_malloc_atomic(t_gc_state *gc, void **root_addr, size_t size);
void       *gc_realloc(t_gc_state *gc, void **root_addr, void *ptr, size_t newsize);

void        gc_collect(t_gc_state *gc);
size_t      gc_get_heap_size(t_gc_state *gc);
void        gc_print_stats(t_gc_state *gc);
