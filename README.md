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
> * This diagram visualizes both the **persistent root-managed heap** and **eagerly collected temporaries**.

---

## Mermaid Diagram: Eager Collection of Temporary Stack Objects

> [!TIP]
> Illustrates temporary stack-local objects being collected once they go out of scope.

```mermaid
flowchart TD
    subgraph FunctionScope
        Temp1([Temporary Object A])
        Temp2([Temporary Object B])
    end

    StartFunc([Function starts]) --> AllocTemp1[Allocate Temp A] --> Temp1
    AllocTemp1 --> AllocTemp2[Allocate Temp B] --> Temp2

    FunctionEnd([Function returns]) --> GCStart([Start GC])
    Temp1 -.-> SweepPhase
    Temp2 -.-> SweepPhase
    SweepPhase --> End([Function scope cleaned])
```

> [!NOTE]
>
> * Temporary objects are **not registered as roots**.
> * They are freed automatically during sweep after scope exits.

---

## Mermaid Diagram: Allocation & Collection Timeline

> [!TIP]
> Visualizes multiple allocations, GC runs, and heap growth over time (`test_large.c`).

```mermaid
flowchart TD
    A0([Start Program])
    A1([Allocate Block A])
    A2([Allocate Block B])
    A3([Allocate Block C])
    GC1([GC Run 1 - threshold exceeded])
    Sweep1([Sweep unmarked blocks])
    A4([Allocate Block D])
    A5([Allocate Block E])
    Realloc1([Reallocate Block B])
    GC2([GC Run 2 - threshold exceeded])
    Sweep2([Sweep unmarked blocks])
    A6([Allocate Block F])
    End([End Program])

    %% Connect vertically
    A0 --> A1
    A1 --> A2
    A2 --> A3
    A3 --> GC1
    GC1 --> Sweep1
    Sweep1 --> A4
    A4 --> A5
    A5 --> Realloc1
    Realloc1 --> GC2
    GC2 --> Sweep2
    Sweep2 --> A6
    A6 --> End
```

> [!NOTE]
>
> * Each step shows allocation, reallocation, or GC.
> * Sweeping removes unmarked blocks including temporary stack objects.
> * Demonstrates dynamic threshold-triggered garbage collection.

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
