#include "tools.h"

static void write_spatial_csv(const char *path,
                              size_t iters,
                              const test_result_t results[4][2]) {
    FILE *f = fopen(path, "w");
    if (!f) {
        return;
    }
    fprintf(f, "bytes,n_elems,iters,method,seconds,cycles,time_per_elem\n");
    for (size_t idx = 0; idx < SPATIAL_SIZES_COUNT; ++idx) {
        size_t bytes = SPATIAL_SIZES_BYTES[idx];
        size_t n_elems = bytes / sizeof(uint64_t);
        if (n_elems == 0) {
            n_elems = 1;
        }
        for (size_t m = 0; m < 2; ++m) {
            const char *method = (m == 0) ? "sequential" : "chunked";
            double seconds = results[idx][m].seconds;
            double denom = (double)n_elems * (double)iters;
            double time_per_elem = (denom > 0.0) ? (seconds / denom) : 0.0;
            fprintf(f, "%zu,%zu,%zu,%s,%.9f,%llu,%.12e\n",
                    bytes,
                    n_elems,
                    iters,
                    method,
                    seconds,
                    (unsigned long long)results[idx][m].cycles,
                    time_per_elem);
        }
    }
    fclose(f);
}

int main(void) {
    const size_t iters = 10000;
    const size_t line_bytes = 64;

    test_result_t results[4][2];
    spatial_run_all(iters, line_bytes, results);

    write_spatial_csv("output/spatial.csv", iters, results);
    printf("wrote output/spatial.csv\n");
    return 0;
}
