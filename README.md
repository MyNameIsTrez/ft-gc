# ft-gc

> [!WARNING] **Disclaimer:**
> This project was developed with heavy assistance from ChatGPT (OpenAI’s GPT-5 model).
> All generated code and explanations were carefully reviewed, tested, and adjusted manually to ensure correctness, style compliance, and alignment with the 42 school’s C coding standards.

---

A compact, educational **garbage collector** implementation written in C, following the **42 school network’s strict C code style**.
It provides automatic memory management with basic mark-and-sweep collection, manual root registration, and optional debug logging.

> [!TIP] **Debug Mode:**
> Pass the flag `-DGC_DEBUG` when compiling to enable colored debug output showing allocations, collections, and root operations.

---

## Design Overview

> [!NOTE] **Memory Model**

* **Blocks (`t_gc_block`)**: Each allocation is stored in a block containing metadata (`size`, `atomic`, `marked`, pointer to root, next block).
* **Roots (`t_gc_root`)**: Pointers referencing allocated blocks. Linked to blocks for efficient marking.
* **State (`t_gc_state`)**: Tracks all allocated blocks, roots, heap size, and collection thresholds.
* **Stack base**: Recorded at `gc_create` to avoid registering stack-local pointers as persistent roots.

> [!NOTE] **Algorithm**

* **Allocation**:

  * `gc_malloc` and `gc_malloc_atomic` allocate memory blocks.
  * Persistent root registration if pointer is provided and not stack-local.
  * Atomic allocations skip pointer scanning.

* **Reallocation**:

  * `gc_realloc` creates a new block, copies existing data, updates roots, frees old block.

* **Garbage Collection**:

  * **Mark phase**: Traverses roots, marks reachable blocks.
  * **Sweep phase**: Frees unmarked blocks.
  * **Thresholds**: Runs automatically when payload exceeds a dynamic threshold.

* **Root Management**:

  * Stack-local pointers ignored, allowing temporary objects to be collected.
  * Persistent roots (globals/statics) must be explicitly registered.

---

## Diagram of GC Internals

```
+-----------------+           +-----------------+
|   Root List     | --------> |   t_gc_block 1  |
| (persistent ptr)|           +-----------------+
|                 |           | payload         |
|                 |           | size            |
|                 |           | marked          |
+-----------------+           | atomic          |
                              | next -> ...     |
                              +-----------------+
                              |   t_gc_block 2  |
                              +-----------------+
                              | payload         |
                              | size            |
                              | marked          |
                              | atomic          |
                              | next -> ...     |
                              +-----------------+
```

> [!TIP] **Collection Process**
>
> 1. **Mark phase**: Traverse roots → mark reachable blocks (`marked = 1`).
> 2. **Sweep phase**: Traverse blocks → free unmarked blocks.
> 3. Update GC stats (`total_payload`, `last_live`, `next_threshold`).
>    Temporary stack objects are not linked in the root list, so they can be freed once out of scope.

---

## Running Tests

### `test_small.c`

> [!NOTE] Minimal demonstration of allocation, reallocation, and automatic garbage collection.

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_small.c -Igc -o test_small && ./test_small
```

Expected behavior:

* Allocates a few integers.
* Reallocates some blocks to test root updates.
* Calls `gc_collect()` to clean unused objects.
* Final stats show remaining live blocks.

---

### `test_eager.c`

> [!NOTE] Demonstrates **eager collection** of temporary stack-scoped objects.

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_eager.c -Igc -o test_eager && ./test_eager
```

Expected behavior:

* Allocates an object inside a helper function.
* Forces garbage collection after the function returns.
* Temporary object is freed immediately.
* Subsequent allocations show proper heap reuse.

---

### `test_large.c`

> [!NOTE] Stress test to validate GC behavior under heavy load.

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_large.c -Igc -o test_large && ./test_large
```

Expected behavior:

* Allocates tens of thousands of small blocks.
* Mix of atomic and non-atomic allocations.
* Periodic `realloc` operations test root updates.
* GC keeps heap size under control.
* Final stats display live block count and payload size.

---

## Notes

> [!TIP] Key points

* Mark-and-sweep garbage collector with explicit root management.
* Handles both atomic and non-atomic allocations.
* Temporary stack allocations are automatically collectible.
* Single-threaded; not suitable for multi-threaded production use.
* Debug output provides detailed allocation, collection, and root tracking.
