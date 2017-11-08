/*
 * Test kernels, for vectorising
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <getopt.h>
#include <stdbool.h>

#define BYTE_OPS     (1<<22)
#define WORD_OPS     (BYTE_OPS/2)
#define SINGLE_OPS   (BYTE_OPS/4)
#define DOUBLE_OPS   (BYTE_OPS/8)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef unsigned long (test_func)(int64_t *start);
#define math_opt_barrier(x)                                     \
    ({ __typeof (x) __x = (x); __asm ("" : "+w" (__x)); __x; })

volatile uint64_t dump;

typedef struct {
    char        *name;
    char        *desc;
    test_func   *func;
} testdef_t;

/* Generate an array of psuedo-random data to feed into our
 * vectorisable kernel. The array needs to be nicely aligned to
 * help the vectoriser
 */
static void * get_aligned_block(size_t size)
{
    void *p;
    if (posix_memalign(&p, 16, size)!=0) {
        fprintf(stderr, "%s: failed to allocate memory\n", __func__);
        abort();
    }
    return p;
}

static void * __attribute__ ((noinline)) get_data(uint32_t seed)
{
    unsigned long i;
    long int rseed = random();
    uint8_t *data = get_aligned_block(BYTE_OPS * sizeof(uint8_t));

    for (i=0; i < BYTE_OPS; i = i + 4)
    {
        data[i]   = ((rseed >>  8) ^ (seed >>  0) ^ i) & 0xff;
        data[i+1] = ((rseed >> 16) ^ (seed >>  8) ^ i) & 0xff;
        data[i+2] = ((rseed >> 24) ^ (seed >> 16) ^ i) & 0xff;
        data[i+3] = ((rseed >>  0) ^ (seed >> 24) ^ i) & 0xff;
    }

    return data;
}


static inline int64_t get_clock(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/* Tests */
static unsigned long bit_fiddle_bytes(int64_t *start)
{
    uint8_t *and, *add, *sub, *xor, *out;
    unsigned long i, j;

    and = __builtin_assume_aligned(get_data(0x1248a33a), 16);
    add = __builtin_assume_aligned(get_data(0xABABCDCD), 16);
    sub = __builtin_assume_aligned(get_data(0xDEEDBEEF), 16);
    xor = __builtin_assume_aligned(get_data(0x7e7e7e7e), 16);
    out = __builtin_assume_aligned(get_aligned_block(BYTE_OPS * sizeof(uint8_t)), 16);

    *start = get_clock();

    /* Twiddle bits */
    for (j = 0; j < 256; j++) {
        for (i = 0; i < BYTE_OPS; i++)
        {
            uint8_t value = out[i];
            value |= i & and[i];
            value += add[i];
            value ^= xor[i];
            value -= sub[i];
            out[i] = value;
        }
    }

    math_opt_barrier(*out);

    return (BYTE_OPS * j * 5);
}

static unsigned long bytewise_xor(int64_t *start)
{
    uint8_t *a, *b, *out;
    unsigned long i, j;

    a = __builtin_assume_aligned(get_data(0x12345678), 16);
    b = __builtin_assume_aligned(get_data(0xDEEDBEEF), 16);
    out = __builtin_assume_aligned(get_aligned_block(BYTE_OPS * sizeof(uint8_t)), 16);

    *start = get_clock();

    /* Twiddle bits */
    for (j = 0; j < 256; j++) {
        for (i = 0; i < BYTE_OPS; i++)
        {
            out[i] = a[i] ^ b[i];
        }
    }

    math_opt_barrier(*out);

    return (BYTE_OPS * j);
}

static unsigned long bytewise_xor_stream(int64_t *start)
{
    uint8_t *a, out = 0;
    unsigned long i, j;

    a = __builtin_assume_aligned(get_data(0x12345678), 16);

    *start = get_clock();

    for (i = 0; i < BYTE_OPS; i++)
    {
        out ^= a[i];
    }

    math_opt_barrier(out);
    dump = out;

    return (BYTE_OPS);
}

static unsigned long wordwise_xor(int64_t *start)
{
    uint16_t *a, *b, *out;
    unsigned long i, j;

    a = __builtin_assume_aligned(get_data(0x12345678), 16);
    b = __builtin_assume_aligned(get_data(0xDEEDBEEF), 16);
    out = __builtin_assume_aligned(get_aligned_block(BYTE_OPS * sizeof(uint8_t)), 16);

    *start = get_clock();

    /* Twiddle bits */
    for (j = 0; j < 256; j++) {
        for (i = 0; i < WORD_OPS; i++)
        {
            out[i] = a[i] ^ b[i];
        }
    }

    math_opt_barrier(*out);

    return (WORD_OPS * j);
}

static unsigned long float32_multiply(int64_t *start)
{
    float *a, *b, *out;
    unsigned long i, j;

    a = (float *) __builtin_assume_aligned(get_data(0xABABCDCD), 16);
    b = (float *) __builtin_assume_aligned(get_data(0xDEEDBEEF), 16);
    out = __builtin_assume_aligned(get_aligned_block(BYTE_OPS * sizeof(uint8_t)), 16);

    *start = get_clock();

    /* Do multiplies */
    for (j = 0; j < 128; j++) {
        for (i = 0; i < SINGLE_OPS; i++)
        {
            out[i] = a[i] * b[i];
        }
    }

    math_opt_barrier(*out);

    return (SINGLE_OPS * j);
}

static testdef_t tests[] = {
    {
        .name = "bytewise-bit-fiddle",
        .desc = "add/xor/sub an array of bytes",
        .func = bit_fiddle_bytes
    },
    {
        .name = "bytewise-xor",
        .desc = "xor two array of bytes",
        .func = bytewise_xor
    },
    {
        .name = "bytewise-xor-stream",
        .desc = "xor array of bytes",
        .func = bytewise_xor_stream
    },
    {
        .name = "wordwise-xor",
        .desc = "xor array of words",
        .func = wordwise_xor
    },
    {
        .name = "float32-mul",
        .desc = "floating point multiply an array of singles",
        .func = float32_multiply
    },
};

testdef_t * get_test(char *name) {
    int i;
    testdef_t *t = NULL;

    for (i = 0; i<ARRAY_SIZE(tests); i++) {
        if (strcmp(tests[i].name, name)==0) {
            t = &tests[i];
            break;
        }
    }
    return t;
}

void usage(void) {
    int i;

    fprintf(stderr, "Usage: vector-benchmark [-n [total]] -b <testname>\n\n");
    fprintf(stderr, "  -n - numbers only\n");
    fprintf(stderr, "Tests:\n");
    for (i = 0; i<ARRAY_SIZE(tests); i++) {
        fprintf(stderr, "%s: %s (%p)\n",
                tests[i].name, tests[i].desc, tests[i].func);
    }
}

int main(int argc, char **argv)
{
    int64_t start, end, elapsed;
    unsigned long ops;
    testdef_t *test;
    bool numbers = false;
    bool total_time = false;

    for (;;) {
        static struct option longopts[] = {
            {"help", no_argument, 0, '?'},
            {"numbers", optional_argument, 0, 'n'},
            {"benchmark", required_argument, 0, 'b'},
            {0, 0, 0, 0}
        };
        int optidx = 0;
        int c = getopt_long(argc, argv, "hb:n::", longopts, &optidx);
        if (c == -1) {
            break;
        }

        switch (c) {
            case 0:
            {
                /* flag set by getopt_long, do nothing */
                break;
            }
            case 'b':
            {
                test = get_test(optarg);
                break;
            }
            case 'n':
            {
                numbers = true;
                break;
            }
            case 'h':
            case '?':
            {
                usage();
                exit(1);
            }
        }
    }

    if (!test) {
        fprintf(stderr, "No test specified!\n");
        exit(1);
    }

    /* start = get_clock(); called by test after setup */
    ops = test->func(&start);
    end = get_clock();

    elapsed = end - start;

    if (numbers) {
        fprintf(stdout, "%ld\n", elapsed/(ops/1000));
    } else {
        fprintf(stdout, "%-20s: test took %ld msec\n", test->name, elapsed/1000);
        fprintf(stdout, "%-20s  %ld ops, ~%ld nsec/kop", "", ops, elapsed/(ops/1000));
    }

    return 0;
}
