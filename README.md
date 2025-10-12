Here’s a **simplified README** that keeps the essential information but trims unnecessary detail and long explanations:

---

# ft-gc

> [!NOTE]
> Developed with guidance from ChatGPT (OpenAI GPT-5 model).
> **Disclaimer for 42 students:** Do **not** blindly copy-paste this code. Use it only for learning and reference.

A compact **mark-and-sweep garbage collector** in C. Supports **manual root registration** and **automatic collection of stack-local objects**. Use `-DGC_DEBUG` to see detailed GC operations.

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

Allocate memory:

```c
int *x = gc_malloc(gc, (void **)&x, sizeof(*x));       // may contain pointers
char *buf = gc_malloc_atomic(gc, (void **)&buf, 256); // atomic, no pointer scanning
```

Force garbage collection:

```c
gc_collect(gc);
```

> Stack-local temporaries are automatically collected when their scope ends.

---

## Tests

All tests are run via the unified script **`tests.sh`**:

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

| Test                   | Purpose                                                        |
| ---------------------- | -------------------------------------------------------------- |
| `test_small.c`         | Minimal demo: allocation, reallocation, automatic GC           |
| `test_stack_collect.c` | Stack-local allocation collection                              |
| `test_large.c`         | Stress test with thousands of allocations and reallocs         |
| `test_transitive.c`    | Transitive GC: blocks reachable via other heap objects survive |
| `test_unrooted.c`      | Confirms unrooted heap allocations are freed                   |
