# Ring Buffer (Fixed-Capacity, Overwrite-on-Full)

## Design Decisions
- **Capacity**: Capacity of the ring buffer needs to be enough that it can store upcoming requests while processing of the past once are done, this system does not finalizes or suggest the capacity the caller decides it before starting, array is used of N size (template<typename T, int N>)
- **Full behavior**: the ring buffer is used in places where the consistency and speed is prefered over processing each data so what is does is if it becomes full it overwrites the data from the head which is being coded in push() of the RingBuffer class , so it overwrites the oldest just like FIFO
- **Empty behavior**: when the ring_buffer is empty , we cant pop anything from a empty buffer so the datatype used here is optional<T> which returns nullopt if pop() is applied on empty ringbuffer 
- **Storage**: and array of typename T and int N is begin declared so it can handle any type of datatype and caller can decide the capacity of th ringbuffer 
- **Wraparound**: for the wrap around i had two option to bring the pointer back to the start of ring_buffer using modulo or bitmask , modulo have a calculation over head but can work with any number of N whereas bitmask is much faster than modulo but the N should be a power of 2 

## Move Semantics
const T& copies the whole object as it sees it as an lvalue and things that it will be used again so it considers the value variable as a non disposable and thus allocates memory and copies data fully creating an overhead
so we use std::move T&& to avoid this overload just as move removes the tag of lvalue from the variable and now compiler see it as temp rvalue and does not copy everything just takes the pointers, capacity and size and make the move before object in a empty but legal state

## Debugging: GDB Session
gdb helps to dry run specific parts of code and seing the change in values of variables at the same time in order to verify code line by line 


## Profiling: strace
strace helps us figure out the latency of our code , the number of system call s and outputs a full table of all time related info and summary of system calls for every aspect of code

## A Real Bug I Caught
what i did is to forget to put a _ in CMAKE_CXX STANDARD it didnt break as
g++ 15.2.0 defaults to C++17 (-std=gnu++17), ensuring full backward compatibility and native support for all stable modern C++ features without triggering the breaking changes introduced in C++20.


## Testing
I coded many use of RingBuffer push and pop and also made cases for FAIL and PASS for the unit testing of CTEST covering the push pop order the empty pop behavior the overwrite correctness and move vs copy overload 













## Block 2: Mutex vs Lock-Free SPSC Queue

### Design
- Mutex version: single std::mutex guards the entire push()/pop() body.
- Lock-free version: head_/tail_ as std::atomic<size_t>, no shared count_ —
  fullness/emptiness derived purely from comparing head_ and tail_.
  One slot deliberately sacrificed (capacity N holds N-1 real items) to keep
  the empty/full states distinguishable, since head_==tail_ is ambiguous
  between them otherwise.

### Why lock-free rejects instead of overwriting on full
Overwrite-on-full would require the producer to also advance head_ — but
head_ is owned exclusively by the consumer thread in this design. Single-writer
ownership per atomic variable is what makes the lock-free version correct
without a mutex; breaking it reintroduces the exact data race atomics were
meant to eliminate. So push() returns false instead.

### Memory ordering
- Own thread's atomic (e.g. consumer reading head_): relaxed — no other
  thread ever writes it, so there's nothing to synchronize.
- Other thread's atomic (e.g. consumer reading tail_): acquire, paired with
  the producer's release store. This guarantees that if the acquire-load
  observes the release-store's value, everything the producer did BEFORE
  that release (writing buffer_[tail_]) is also visible — not just the
  atomic variable itself.

### A real debugging story: the WSL2 yield() anomaly
Initial benchmark (capacity=1024, 100k items) appeared to hang. Added progress
instrumentation to both threads — this ruled out deadlock/livelock immediately,
since both counters were still climbing, just very slowly and non-linearly
(10k items took disproportionately longer than 10x the time for 1k items).
Root cause: with capacity far smaller than item count, the producer hits
"full" constantly and calls std::this_thread::yield() an enormous number of
times. Under WSL2's virtualized scheduler, yield()-triggered context switches
are meaningfully more expensive than on bare-metal Linux, and that overhead
compounds under heavy backpressure. Fixed by sizing capacity (131072) to
avoid pathological retry rates — isolating the benchmark to measure raw
per-operation cost rather than backpressure-retry overhead.

### Results (100,000 items, 3 runs)
| Queue      | ns/op (avg) | ops/sec      |
|------------|-------------|--------------|
| Mutex      | ~186 ns     | ~5.4M ops/s  |
| Lock-free  | ~87 ns      | ~11.5M ops/s |

Lock-free is ~2x faster. Mutex pays for kernel-level lock/unlock and
potential thread context switches under contention; lock-free stays entirely
in user space using hardware atomic instructions (CAS/load/store), with no
syscall in the common (uncontended) path.



## Block 3: False Sharing — Detection and Fix

### The setup
head_ and tail_ (both std::atomic<size_t>, 8 bytes each) were declared
adjacent in memory in the lock-free queue. A CPU cache line is 64 bytes,
so both variables likely shared one cache line despite being written by
different threads (producer writes tail_, consumer writes head_) with
no actual data dependency between them.

### Why this matters
The cache moves data in fixed 64-byte aligned chunks, not per-variable.
When one core writes to its variable, the entire cache line — including
the other thread's unrelated variable — gets invalidated on the other
core, forcing an unnecessary re-fetch. This is pure overhead from
physical memory layout, not from any real synchronization requirement.

### Tooling note: perf unavailable under WSL2
perf installed correctly (v7.0.14) but hardware performance counters
(cycles, cache-misses, etc.) are not exposed to the WSL2 guest kernel —
Hyper-V doesn't pass through the CPU's Performance Monitoring Unit by
default. Confirmed via `perf stat -e cycles,...` failing with "No
supported events found" even under sudo. Fell back to software-based
wall-clock throughput comparison (same method as the Block 2 mutex vs
lock-free benchmark) to demonstrate the effect instead.

### Fix
alignas(64) applied to both head_ and tail_, forcing each onto its own
cache line:
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};

### Results (1,000,000 items, capacity sized to avoid backpressure, 3 runs)
| Version   | ns/op (range) |
|-----------|---------------|
| Unpadded  | 94.5 - 99.5   |
| Padded    | 73.6 - 80.2   |

Padded version is ~18-22% faster. Verified the gap wasn't backpressure-driven
by re-running with capacity sized well above item count so push() never
fails/retries — the gap held steady, confirming false sharing (not yield()
overhead) as the cause.