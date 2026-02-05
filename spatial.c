#include "tools.h"

#define KB 1024ULL
#define MB (1024ULL * KB)

// Test sizes (bytes): L1=16KB, L2=128KB, L3=4MB, DRAM=32MB
const size_t SPATIAL_SIZES_BYTES[4] = {
    16ULL * KB,
    128ULL * KB,
    4ULL * MB,
    32ULL * MB,
};

const size_t SPATIAL_SIZES_COUNT = 4;

// Method 1: scan full array each iteration
// - total accesses: n_elems * iters
test_result_t spatial_sequential(size_t n_elems, size_t iters) {
    test_result_t r = {0};
    uint64_t *data = (uint64_t *)alloc_aligned(n_elems * sizeof(uint64_t), 64);
    if (!data) {
        return r;
    }

    for (size_t i = 0; i < n_elems; ++i) {
        data[i] = (uint64_t)i;
    }

    uint64_t sum = 0;
    measure_t m = measure_begin();
    for (size_t it = 0; it < iters; ++it) {
        for (size_t i = 0; i < n_elems; ++i) {
            sum += data[i];
        }
    }
    measure_end(&m);

    r.seconds = measure_seconds(&m);
    r.cycles = measure_cycles(&m);
    r.sum = sum;

    free(data);
    return r;
}

// Method 2: split into 64B chunks, repeat on each chunk before moving on
// - total accesses: n_elems * iters
test_result_t spatial_chunked(size_t n_elems, size_t iters, size_t line_bytes) {
    test_result_t r = {0};
    uint64_t *data = (uint64_t *)alloc_aligned(n_elems * sizeof(uint64_t), 64);
    if (!data) {
        return r;
    }

    for (size_t i = 0; i < n_elems; ++i) {
        data[i] = (uint64_t)i;
    }

    size_t elems_per_line = line_bytes / sizeof(uint64_t);
    if (elems_per_line == 0) {
        elems_per_line = 1;
    }

    uint64_t sum = 0;
    measure_t m = measure_begin();
    for (size_t base = 0; base < n_elems; base += elems_per_line) {
        size_t limit = base + elems_per_line;
        if (limit > n_elems) {
            limit = n_elems;
        }
        for (size_t it = 0; it < iters; ++it) {
            for (size_t i = base; i < limit; ++i) {
                sum += data[i];
            }
        }
    }
    measure_end(&m);

    r.seconds = measure_seconds(&m);
    r.cycles = measure_cycles(&m);
    r.sum = sum;

    free(data);
    return r;
}

// Run both methods for all test sizes

void spatial_run_all(size_t iters, size_t line_bytes, test_result_t out[4][2]) {
    for (size_t idx = 0; idx < SPATIAL_SIZES_COUNT; ++idx) {
        size_t bytes = SPATIAL_SIZES_BYTES[idx];
        size_t n_elems = bytes / sizeof(uint64_t);
        if (n_elems == 0) {
            n_elems = 1;
        }
        out[idx][0] = spatial_sequential(n_elems, iters);
        out[idx][1] = spatial_chunked(n_elems, iters, line_bytes);
    }
}
