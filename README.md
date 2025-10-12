# ft-gc

> [!NOTE]
> Developed with guidance from ChatGPT (OpenAI GPT-5 model). All generated code and explanations were manually reviewed and adapted to fit the 42 C coding style.

A compact, educational **garbage collector** written in C. Implements **mark-and-sweep GC**, manual root registration, and **eager collection of temporary stack objects**. Debug mode outputs colored logging of allocations, frees, and roots.

> [!TIP]
> Compile with `-DGC_DEBUG` to see detailed GC operations in color.

---

## Design Overview

### Memory Model

* **Blocks (`t_gc_block`)**: Each allocation has metadata including `size`, `atomic` flag, `marked`, payload pointer, root pointer, and next block pointer.
* **Roots (`t_gc_root`)**: Persistent pointers into the heap for marking reachable objects.
* **State (`t_gc_state`)**: Tracks blocks, roots, total payload, last live size, next collection threshold, and an approximate stack base.
* **Stack base**: Recorded at `gc_create()` to detect stack-local pointers and prevent registering them as persistent roots.

---

## Core Principles

1. **Allocation**

   * `gc_malloc` / `gc_malloc_atomic` allocate memory and optionally register a persistent root.
   * Temporary stack-local allocations (inside function scopes) are not registered as roots and are collectible once they go out of scope.

2. **Reallocation**

   * `gc_realloc` creates a new block and copies data.
   * If the old block had a root, the root is updated to point to the new block.
   * Old block is freed immediately after reallocation.

3. **Garbage Collection**

   * Triggered manually via `gc_collect()` or automatically if `total_payload > last_live + next_threshold`.
   * **Mark-and-sweep strategy**:

     * **Mark**: Traverse roots and mark reachable blocks.
     * **Sweep**: Free unmarked blocks.
   * Updates `last_live` and calculates `next_threshold` as `max(2*last_live, GC_DEFAULT_THRESHOLD)`.

4. **Root Management**

   * Only non-stack addresses are registered as persistent roots.
   * Stack-local variables are automatically collectible after their function scope exits.
   * Atomic allocations (via `gc_malloc_atomic`) are not interpreted as containing pointers, but roots still track their block if provided.

---

## Garbage Collector Lifecycle

```mermaid
flowchart TD
    subgraph Roots
        R1([Root A])
        R2([Root B])
    end

    B1["<span style='color:green'>Block A (marked)</span>"]
    B2["<span style='color:red'>Block B (swept)</span>"]
    B3["<span style='color:green'>Block C (marked)</span>"]
    B4["<span style='color:yellow'>Block B reallocated</span>"]

    subgraph TempStack
        T1["<span style='color:orange'>Temp Object 1</span>"]
        T2["<span style='color:orange'>Temp Object 2</span>"]
    end

    R1 --> B1
    R2 --> B3

    B1 --> B2
    B2 --> B3
    B3 --> B4

    Alloc1([Allocate Block A]) --> Alloc2([Allocate Block B]) --> RootReg1([Register Roots])
    RootReg1 --> GCCheck1{Heap > threshold?}
    GCCheck1 -- Yes --> GCStart([Start GC]) --> MarkPhase([Mark reachable blocks])
    MarkPhase --> SweepPhase([Free unmarked blocks]) --> StatsUpdate([Update GC stats])
    StatsUpdate --> Alloc3([Allocate Block C]) --> Realloc1([Reallocate Block B])
    Realloc1 --> RootUpdate([Update roots after realloc])
    RootUpdate --> GCCheck2{Heap > threshold?}
    GCCheck2 -- Yes --> GCStart

    StartFunc([Function start]) --> T1 --> T2
    FunctionEnd([Function ends]) --> GCStart
    T1 -.-> SweepPhase
    T2 -.-> SweepPhase

    StatsUpdate --> End([Program End])
```

> **Color code:**
> *Green* = live/persistent blocks
> *Red* = freed blocks
> *Yellow* = reallocated blocks
> *Orange* = temporary stack-local objects

---

## Eager Collection of Temporaries (`test_eager.c`)

```mermaid
flowchart TD
    subgraph FunctionScope
        Temp1([Temporary Object A])
        Temp2([Temporary Object B])
    end

    StartFunc([Function starts]) --> AllocTemp1[Allocate Temp A] --> Temp1
    AllocTemp1 --> AllocTemp2[Allocate Temp B] --> Temp2

    FunctionEnd([Function returns]) --> GCStart([Run gc_collect])
    Temp1 -.-> SweepPhase
    Temp2 -.-> SweepPhase
    SweepPhase --> End([Function scope cleaned])
```

> Temporary stack objects are **not added as roots** and are freed automatically after `gc_collect()`.

---

## Reallocation & Root Update

```mermaid
flowchart TD
    OldBlock[Old Block]
    NewBlock[New Block - after gc_realloc]
    Root[Root pointer]

    Root --> OldBlock
    gc_realloc([gc_realloc called]) --> NewBlock
    OldBlock -.-> FreeOld[Old block freed]
    Root --> NewBlock
```

> * Demonstrates how `gc_realloc` creates a new block, copies data, frees the old block, and updates any root pointing to the old block.

---

## Allocation & Collection Timeline (`test_large.c`)

```mermaid
flowchart TD
    Start[Start Program & create GC state]

    IterStart[Iteration i]
    AllocP[gc_malloc p]
    AllocQ[gc_malloc_atomic q]
    StoreValues[Store values in p and q]
    ReallocQ[q = gc_realloc q]
    ThresholdCheck[Heap > next_threshold?]
    AutoGC[Automatic GC triggered]
    ProgressPrint[Print progress every 10,000 iterations]
    IterEnd[End Iteration]

    FinalCollect[Final gc_collect]
    PrintStats[Print final stats]
    DestroyGC[gc_destroy]
    End[Program End]

    Start --> IterStart
    IterStart --> AllocP
    AllocP --> AllocQ
    AllocQ --> StoreValues
    StoreValues --> ReallocQ
    ReallocQ --> ThresholdCheck
    ThresholdCheck -- Yes --> AutoGC --> IterEnd
    ThresholdCheck -- No --> IterEnd
    IterEnd --> ProgressPrint --> IterStart
    IterStart --> FinalCollect
    FinalCollect --> PrintStats --> DestroyGC --> End
```

---

## Running Tests

### `test_small.c` — Minimal demonstration

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_small.c -Igc -o test_small
./test_small
```

*Shows allocation, reallocation, automatic GC, and final heap stats.*

---

### `test_eager.c` — Eager collection demonstration

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_eager.c -Igc -o test_eager
./test_eager
```

*Confirms temporary stack objects are collected immediately after function scope exits.*

---

### `test_large.c` — Stress test

```sh
gcc -DGC_DEBUG -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined gc/*.c test_large.c -Igc -o test_large
./test_large
```

*Allocates tens of thousands of small blocks, mixes atomic and non-atomic allocations, reallocations, and automatic GC.*

---

## Notes

* Mark-and-sweep GC with manual root management.
* Handles atomic and non-atomic allocations.
* Temporary stack allocations are automatically collectible.
* Realloc updates roots correctly.
* Single-threaded, not suitable for multi-threaded production.
* Debug output provides detailed allocation, collection, and root tracking.
