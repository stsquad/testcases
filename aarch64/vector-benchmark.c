/*
 * Test kernel, for vectorising
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#define OPS     (1<<22)
#define ALIGNED_OPS (OPS/(sizeof(uint64_t)/(sizeof(uint8_t))))

/* Generate an array of psuedo-random data to feed into our
 * vectorisable kernel. The array needs to be nicely aligned to
 * help the vectoriser
 */
static uint8_t * get_data(uint32_t seed)
{
    unsigned long i;
    long int rseed = random();
    const int total = OPS + 0x100;
    uint8_t *data = malloc(total * sizeof(uint8_t));

    fprintf(stderr, "%s: with %#" PRIx32 "/%#lx\n", __func__, seed, rseed);

    for (i=0; i < total; i = i + 4)
    {
        data[i]   = ((rseed >>  8) ^ (seed >>  0) ^ i) & 0xff;
        data[i+1] = ((rseed >> 16) ^ (seed >>  8) ^ i) & 0xff;
        data[i+2] = ((rseed >> 24) ^ (seed >> 16) ^ i) & 0xff;
        data[i+3] = ((rseed >>  0) ^ (seed >> 24) ^ i) & 0xff;
    }

    return data;
}

static inline uint8_t * align_data(uint8_t *data)
{
    uintptr_t ptr = (uintptr_t) data;
    ptr = (ptr + (8 - 1)) & -8;
    return (uint8_t *) ptr;
}

static inline int64_t get_clock(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

int main(void)
{
    uint8_t *add, *sub, *or, *xor, *out;
    int64_t start, end;
    unsigned long i, j;

    add = align_data(get_data(0xABABCDCD));
    sub = align_data(get_data(0xDEEDBEEF));
    or  = align_data(get_data(0x10101010));
    xor = align_data(get_data(0x7e7e7e7e));
    out = align_data(get_data(0x00000000));

    start = get_clock();

    /* Twiddle bits */
    for (j = 0; j < 100; j++) {
        for (i = 0; i < OPS; i++)
        {
            uint8_t value = out[i];
            value += add[i];
            value ^= xor[i];
            value -= sub[i];
            value |= or[i];
            value ^= xor[i];
            out[i] = value;
        }
    }

    end = get_clock();

    fprintf(stderr, "%s: ran %ld x %ld cycles in %ld\n", __func__, i, j, end - start);

    return 0;
}
