/*
 * gcc-8 -g3 -O3 loop-interchange.c -o loop-interchange
 * gcc-8 -g3 -O3 -march=armv8.2-a+sve loop-interchange.c -o loop-interchange-sve
 * gcc-8 -g3 -O3 -no-loop-interchange loop-interchange.c -o loop-interchange
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define math_opt_barrier(x)                                     \
    ({ __typeof (x) __x = (x); __asm ("" : "+w" (__x)); __x; })

#define OPS 100

static void * get_aligned_block(size_t size)
{
    void *p;
    if (posix_memalign(&p, 16, size)!=0) {
        fprintf(stderr, "%s: failed to allocate memory\n", __func__);
        abort();
    }
    return p;
}

static unsigned int float32_multiply()
{
    float *a, *b, *out;
    unsigned long i, j;

    a = (float *) __builtin_assume_aligned(get_aligned_block(OPS * sizeof(float)), 16);
    b = (float *) __builtin_assume_aligned(get_aligned_block(OPS * sizeof(float)), 16);
    out = __builtin_assume_aligned(get_aligned_block(OPS * sizeof(float)), 16);

    /* Do multiplies */
    for (j = 0; j < 128; j++) {
        for (i = 0; i < OPS; i++)
        {
            out[i] = a[i] * b[i];
        }
    }

    math_opt_barrier(*out);

    return (OPS * j);
}

int main(int argc, char **argv)
{
    return float32_multiply();
}
