#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mach/mach_time.h>

// Measurement helpers

typedef struct {
    uint64_t ticks_start;
    uint64_t ticks_end;
    uint64_t cycles_start;
    uint64_t cycles_end;
} measure_t;

typedef struct {
    double seconds;
    uint64_t cycles;
    uint64_t sum;
} test_result_t;

static inline uint64_t read_ticks(void) {
    return mach_absolute_time();
}

static inline double ticks_to_seconds(uint64_t ticks) {
    static mach_timebase_info_data_t info = {0, 0};
    if (info.denom == 0) {
        (void)mach_timebase_info(&info);
    }
    double nanos = (double)ticks * (double)info.numer / (double)info.denom;
    return nanos / 1e9;
}

static inline uint64_t read_cycles(void) {
#if defined(__x86_64__)
    return __builtin_ia32_rdtsc();
#elif defined(__aarch64__)
    uint64_t val;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(val));
    return val;
#else
    return 0;
#endif
}

static inline measure_t measure_begin(void) {
    measure_t m;
    m.ticks_start = read_ticks();
    m.cycles_start = read_cycles();
    m.ticks_end = 0;
    m.cycles_end = 0;
    return m;
}

static inline void measure_end(measure_t *m) {
    m->ticks_end = read_ticks();
    m->cycles_end = read_cycles();
}

static inline double measure_seconds(const measure_t *m) {
    return ticks_to_seconds(m->ticks_end - m->ticks_start);
}

static inline uint64_t measure_cycles(const measure_t *m) {
    return m->cycles_end - m->cycles_start;
}

// Aligned allocation helper

static inline void *alloc_aligned(size_t size, size_t alignment) {
    void *ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
    memset(ptr, 0, size);
    return ptr;
}

// Spatial prototypes

extern const size_t SPATIAL_SIZES_BYTES[4];
extern const size_t SPATIAL_SIZES_COUNT;

test_result_t spatial_sequential(size_t n_elems, size_t iters);

test_result_t spatial_chunked(size_t n_elems, size_t iters, size_t line_bytes);

void spatial_run_all(size_t iters, size_t line_bytes, test_result_t out[4][2]);

#endif
