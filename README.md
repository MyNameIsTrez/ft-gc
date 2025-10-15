# ft-gc

> [!CAUTION]
> This garbage collector repository isn't usable.
> It uses the stack address of variables to automatically determine lifetimes, but that's Undefined Behavior according to the C standard, and so it doesn't work with AddressSanitizer.

> [!NOTE]
> Developed with guidance from ChatGPT (OpenAI GPT-5 model).

> [!WARNING]
> **Disclaimer for 42 students:** Do **not** blindly copy-paste this code. Use it only for learning and reference.

A compact **mark-and-sweep garbage collector** in C. Supports:

* **Manual root registration** — all allocations must pass a variable address to register a root
* **Automatic collection** of GC-managed blocks when `gc_collect()` is called
* **Atomic allocations** — blocks that do not contain pointers are not scanned
* **Strict allocation rules** — `gc_malloc()`, `gc_malloc_atomic()`, and `gc_realloc()` require valid roots; errors are reported if rules are violated

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

**Persistent (rooted) allocations** — pass a pointer to register as a root:

```c
int *x = gc_malloc(gc, (void **)&x, sizeof(*x));       // may contain pointers
char *buf = gc_malloc_atomic(gc, (void **)&buf, 256); // atomic, no pointer scanning
```

> ⚠ Root registration is required. Passing `NULL` as the root returns `NULL` and prints a `[GC ERROR]` message.

---

### Force Garbage Collection

```c
gc_collect(gc);
```

> Stack-local allocations remain collectible only when `gc_collect()` is called.

---

### Reallocation

```c
q = gc_realloc(gc, q, new_size);
```

* Updates root pointer automatically if one exists
* Old block is freed after copying data
* Must be called on GC-managed blocks; reallocating a non-GC pointer returns `NULL` and prints `[GC ERROR]`
* Reallocating `NULL` is not allowed — use `gc_malloc()` with a valid root instead

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
* Errors `[GC ERROR]` red

### Available Tests

| Test                   | Purpose                                                            |
| ---------------------- | ------------------------------------------------------------------ |
| `test_small.c`         | Minimal demo: allocation, reallocation, automatic GC               |
| `test_stack_collect.c` | Stack-local allocation collection (requires `gc_collect()`)        |
| `test_large.c`         | Stress test with hundreds of thousands of allocations and reallocs |
| `test_unrooted.c`      | Confirms that allocations with `NULL` root are rejected            |
