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

## Combined Master Lifecycle Diagram

> [!TIP]
> Green = live/marked blocks, Red = freed/swept blocks, Yellow = reallocated blocks, Orange = temporary stack-local objects.

```mermaid
flowchart TD
    %% Persistent Roots
    subgraph Roots
        R1([Root A])
        R2([Root B])
    end

    %% Heap blocks
    B1["<span style='color:green'>Block A (marked)</span>"]
    B2["<span style='color:red'>Block B (swept)</span>"]
    B3["<span style='color:green'>Block C (marked)</span>"]
    B4["<span style='color:yellow'>Block B reallocated</span>"]

    %% Temporary stack objects
    subgraph TempStack
        T1["<span style='color:orange'>Temp Object 1</span>"]
        T2["<span style='color:orange'>Temp Object 2</span>"]
    end

    %% Root connections
    R1 --> B1
    R2 --> B3

    %% Heap links
    B1 --> B2
    B2 --> B3
    B3 --> B4

    %% Allocation flow
    Alloc1([Allocate Block A]) --> Alloc2([Allocate Block B]) --> RootReg1([Register Roots A & B])
    RootReg1 --> GCCheck1{Heap > threshold?}
    GCCheck1 -- Yes --> GCStart([Start GC]) --> MarkPhase([Mark Phase: mark reachable blocks])
    MarkPhase --> SweepPhase([Sweep Phase: free unmarked blocks]) --> StatsUpdate([Update GC stats])
    StatsUpdate --> Alloc3([Allocate Block C]) --> Realloc1([Reallocate Block B]) --> RootUpdate([Update Roots after realloc])
    RootUpdate --> GCCheck2{Heap > threshold?}
    GCCheck2 -- Yes --> GCStart

    %% Temporary stack allocations
    StartFunc([Function start: allocate temporaries]) --> T1 --> T2
    FunctionEnd([Function end: scope exits]) --> GCStart
    T1 -.-> SweepPhase
    T2 -.-> SweepPhase

    %% Program end
    StatsUpdate --> End([Program End])
```

> [!NOTE]
>
> * **Green blocks** = live/marked persistent allocations.
> * **Red blocks** = unmarked, freed during sweep.
> * **Yellow blocks** = reallocated blocks with updated roots.
> * **Orange blocks** = temporary stack-local objects, automatically freed when scope ends.
> * This diagram visualizes both the **persistent root-managed heap** and **eagerly collected temporaries** in one view.

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
> Confirms stack-scoped objects are collectible.

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
