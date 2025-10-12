#pragma once
#include "gc.h"
#include <stdint.h>
#include <stdio.h>

#ifdef GC_DEBUG
# define GC_LOG(fmt, ...) fprintf(stderr, "[minigc] " fmt "\n", ##__VA_ARGS__)
#else
# define GC_LOG(fmt, ...) do {} while(0)
#endif

t_gc_block *gc_create_block(t_gc_state *gc, size_t size, int atomic);
void        gc_free_block(t_gc_state *gc, t_gc_block *prev, t_gc_block *block);
t_gc_block *gc_find_block(t_gc_state *gc, uintptr_t ptr);
void        gc_add_root(t_gc_state *gc, void **addr, t_gc_block *block);
void        gc_mark_from_roots(t_gc_state *gc);
void        gc_sweep(t_gc_state *gc);
