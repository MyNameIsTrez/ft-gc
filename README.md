# ft-gc

> [!NOTE]
> Developed with guidance from ChatGPT (OpenAI GPT-5 model).

> [!WARNING]
> **Disclaimer for 42 students:** Do **not** blindly copy-paste this code. Use it only for learning and reference.

A compact **mark-and-sweep garbage collector** in C. Supports:

* **Manual root registration** (for heap persistence)
* **Collection of unrooted allocations** (including stack-local objects) when `gc_collect()` is called

Use `-DGC_DEBUG` to see detailed GC operations.

---

## Quick Start

Include the GC in your project:

```c
#include "gc.h"
```

Initialize and destroy:

```c
t_gc_state *gc = gc_create();
...
gc_destroy(gc);
```

---

### Allocate Memory

**Persistent (rooted) allocations** — must pass a pointer to register as a root:

```c
int *x = gc_malloc(gc, (void **)&x, sizeof(*x));       // may contain pointers
char *buf = gc_malloc_atomic(gc, (void **)&buf, 256); // atomic, no pointer scanning
```

> ⚠ Root registration is required for the object to survive garbage collection. If you pass `NULL` as the root, the allocation is **unrooted** and may be collected on the next `gc_collect()`.

---

### Force Garbage Collection

```c
gc_collect(gc);
```

> Stack-local and unrooted allocations that are no longer referenced can be freed during a collection. This only happens when you call `gc_collect()` — nothing runs automatically on scope exit.

---

### Reallocation

```c
q = gc_realloc(gc, q, new_size);
```

* Updates root pointer automatically if one exists
* Old block is freed after copying data
* If the pointer was not allocated by GC, standard `realloc()` is used

---

## Tests

Run all tests via the unified script:

```sh
chmod +x tests.sh
./tests.sh            # run all tests
./tests.sh test_small # run a specific test
```

**Color scheme:**

* `[TEST_*]` cyan
* Allocation info blue
* GC actions magenta
* Pass/fail messages green/red

### Available Tests

| Test                   | Purpose                                                            |
| ---------------------- | ------------------------------------------------------------------ |
| `test_small.c`         | Minimal demo: allocation, reallocation, automatic GC               |
| `test_stack_collect.c` | Stack-local allocation collection (requires `gc_collect()`)        |
| `test_large.c`         | Stress test with hundreds of thousands of allocations and reallocs |
| `test_transitive.c`    | Transitive GC: blocks reachable via other heap objects survive     |
| `test_unrooted.c`      | Confirms unrooted heap allocations are freed                       |
