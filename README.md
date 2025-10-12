# ft-gc

> [!NOTE]
> This project was developed with heavy assistance from ChatGPT (OpenAI’s GPT-5 model).
> All generated code and explanations were carefully reviewed, tested, and adjusted manually to ensure correctness, style compliance, and alignment with the 42 school’s C coding standards.

---

A compact, educational **garbage collector** implementation written in C, following the **42 school network’s strict C code style**.
It provides automatic memory management with basic mark-and-sweep collection, manual root registration, and optional debug logging.

> [!TIP]
> Pass the `-DGC_DEBUG` flag when compiling to enable colored debug output showing allocations, collections, and root operations.

---

## Design Overview

### Memory Model

* **Blocks (`t_gc_block`)**: Metadata per allocation: `size`, `atomic`, `marked`, payload pointer, link to root, next block.
* **Roots (`t_gc_root`)**: Pointers referencing allocated blocks for marking.
* **State (`t_gc_state`)**: Tracks allocated blocks, roots, heap size, and collection thresholds.
* **Stack base**: Approximate stack address recorded at `gc_create` to avoid registering stack-local pointers as persistent roots.

### Algorithm

* **Allocation**

  * `gc_malloc` and `gc_malloc_atomic` allocate memory blocks.
  * Registers persistent root only if pointer is not stack-local.
  * Atomic blocks skip pointer scanning.

* **Reallocation**

  * `gc_realloc` creates a new block, copies data, updates roots, frees old block.

* **Garbage Collection**

  * **Mark phase**: Traverse roots, mark reachable blocks.
  * **Sweep phase**: Free blocks that are unmarked.
  * **Thresholds**: Runs automatically if payload exceeds a dynamic threshold.

* **Root Management**

  * Stack-local pointers are ignored to allow temporary objects to be collected after scope exit.
  * Persistent roots (globals/statics) must be registered.

---

## Mark-and-Sweep Diagram

> [!NOTE]
> The following diagram illustrates the GC process. Green blocks are **marked** (live), red blocks are **swept** (freed).

```
Root List
+-------------+
|  root1 ---> |---------------------------+
|  root2 ---> |                           |
+-------------+                           |
                                          |
          +-----------------+             |
          |  t_gc_block 1   |  marked ✅   |
          | payload: 0x1000 |------------>|
          | size: 4         |             |
          | marked: 1       |             |
          | atomic: 0       |             |
          +-----------------+             |
          |  t_gc_block 2   |  marked ❌  | <-- swept
          | payload: 0x1010 |             |
          | size: 8         |             |
          | marked: 0       |             |
          | atomic: 1       |             |
          +-----------------+             |
          |  t_gc_block 3   |  marked ✅  |
          | payload: 0x1020 |------------>|
          | size: 12        |
          | marked: 1       |
          | atomic: 0       |
          +-----------------+
```

> [!TIP]
> **Mark phase**: Traverse roots → mark reachable blocks (`marked = 1`).
> **Sweep phase**: Traverse blocks → free unmarked blocks. Temporary stack objects are not linked in the root list and are collectible once out of scope.

---

## Running Tests

### `test_small.c`

**Purpose:** Minimal demonstration of allocation, reallocation, and automatic garbage collection.

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_small.c -Igc -o test_small && ./test_small
```

> [!TIP]
> Final stats show remaining live blocks after `gc_collect()`.

---

### `test_eager.c`

**Purpose:** Demonstrates **eager collection** of temporary stack-scoped objects.

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_eager.c -Igc -o test_eager && ./test_eager
```

> [!IMPORTANT]
> Temporary objects created inside a function are freed immediately after the function returns when `gc_collect()` is called.
> This confirms stack-scoped objects are collectible.

---

### `test_large.c`

**Purpose:** Stress test to validate GC behavior under heavy load.

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_large.c -Igc -o test_large && ./test_large
```

> [!WARNING]
> Allocates tens of thousands of small blocks. Monitor memory usage if running on low-RAM environments.

> [!TIP]
> Mix of atomic and non-atomic allocations tests proper root tracking and heap management.
> Final stats display live block count and payload size.

---

## Notes

> [!NOTE]
>
> * Mark-and-sweep GC with explicit root management.
> * Handles both atomic and non-atomic allocations.
> * Temporary stack allocations are automatically collectible.
> * Single-threaded; not suitable for multi-threaded production use.
> * Debug output provides detailed allocation, collection, and root tracking.
