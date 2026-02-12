# Cache Locality Benchmark

This project benchmarks two memory-access patterns on a 32MB byte array where each element is read 10,000 times.

- `method_a`: scans from the first element to the last element, and repeats that full scan 10,000 times.
- `method_b`: reads each element 10,000 times before moving to the next element.

The goal is to observe how loop ordering changes cache behavior across the L1/L2/L3 hierarchy.
Caches fetch memory in cache-line units (64 bytes on this machine), so access pattern matters even when total arithmetic work is similar.

Test machine (author's environment): Intel MacBook Pro
- L1I: 32KB
- L1D: 32KB
- L2: 256KB
- L3: 6MB
- Cache line size: 64B

## Files

- `method_a.c`: full-array scan repeated 10,000 times.
- `method_b.c`: per-element repeated reads before advancing.
- `Makefile`: builds both executables.

Note: The only difference between `method_a.c` and `method_b.c` is the swapped order of the two nested loops (currently around lines 49 and 50).

## Setup

- Array size: `32MB` (`ARRAY_SIZE_BYTES`)
- Repeat count: `10000` (`REPEAT_COUNT`)
- Eviction buffer: `256MB` (`EVICT_SIZE_BYTES`)
- Timer: `clock_gettime(CLOCK_MONOTONIC)`

## Design Notes

1. A 256MB `evict` buffer is used before timing to reduce residual cache state from initialization.
2. The `evict` sweep steps by 64 bytes so each access touches a new cache line, which is sufficient for cache-line-level eviction behavior.
3. A `volatile` sink is used in the eviction loop so the compiler cannot remove that loop as dead code.
4. The measured load path reads through `volatile uint8_t*` so repeated loads are not collapsed into a single multiply-style expression by compiler optimization.

## Build

```bash
make
```

Outputs:
- `./method_a`
- `./method_b`

Clean:

```bash
make clean
```

## Run

```bash
./method_a
./method_b
```

Example output:

```text
 % ./method_a
elapsed: 58.863124 sec
 % ./method_b
elapsed: 49.046835 sec
```

## Notes

- Results can vary significantly by CPU/cache architecture, power management, temperature, and background load.
- Prefer multiple runs and compare average/median values instead of relying on a single run.
