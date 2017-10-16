/*
 * Test kernel, for vectorising
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#define OPS     (1<<15)
#define ALIGNED_OPS (OPS/(sizeof(uint64_t)/(sizeof(uint8_t))))

int main(void)
{
    uint8_t *inA, *inB, *out;
    time_t start, end;
    int i;

    inA = calloc(ALIGNED_OPS, sizeof(uint64_t));
    inB = calloc(ALIGNED_OPS, sizeof(uint64_t));
    out = calloc(ALIGNED_OPS, sizeof(uint64_t));

    /* Seed inA/B */
    for (i = 0; i < OPS; i++)
    {
        inA[i] = i % 129;
        inB[i] = i % 126;
    }

    start = time(NULL);
    /* Twiddle bits */
    for (i = 0; i < OPS; i++)
    {
        out[i] = inA[i] ^ inB[i];
    }


    return 0;
}
