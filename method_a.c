#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE_BYTES (32u * 1024u * 1024u)
#define EVICT_SIZE_BYTES (256u * 1024u * 1024u)
#define REPEAT_COUNT 10000u

int main(void) {
    uint8_t *arr = (uint8_t *)malloc(ARRAY_SIZE_BYTES);
    uint8_t *evict = (uint8_t *)malloc(EVICT_SIZE_BYTES);
    if (arr == NULL || evict == NULL) {
        perror("malloc failed");
        free(arr);
        free(evict);
        return 1;
    }

    memset(arr, 0, ARRAY_SIZE_BYTES);
    memset(evict, 1, EVICT_SIZE_BYTES);

    volatile uint64_t sink = 0;

    // Step by 64 bytes (typical cache-line size) so each access touches a new cache line.
    for (size_t i = 0; i < EVICT_SIZE_BYTES; i += 64) {
        sink += evict[i];
    }

    struct timespec t0, t1;
    if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0) {
        perror("clock_gettime start failed");
        free(arr);
        free(evict);
        return 1;
    }

    // Read through a volatile pointer so each inner-loop access remains a real load.
    // Without this, the compiler can notice that arr[i] does not change inside
    // the inner loop and reduce:
    //     local_sum += arr[i]   (repeated REPEAT_COUNT times)
    // into something like:
    //     local_sum += arr[i] * REPEAT_COUNT
    // That transformation would drastically reduce memory reads and invalidate
    // the intended cache-locality benchmark for method_a.
    volatile uint8_t *v = arr;
    uint64_t local_sum = 0;
    for (size_t r = 0; r < REPEAT_COUNT; ++r) {
        for (size_t i = 0; i < ARRAY_SIZE_BYTES; ++i) {
            local_sum += v[i];
        }
    }
    sink += local_sum;

    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0) {
        perror("clock_gettime end failed");
        free(arr);
        free(evict);
        return 1;
    }

    double elapsed_sec = (double)(t1.tv_sec - t0.tv_sec) +
                         (double)(t1.tv_nsec - t0.tv_nsec) / 1000000000.0;
    printf("elapsed: %.6f sec\n", elapsed_sec);

    free(arr);
    free(evict);
    return 0;
}
