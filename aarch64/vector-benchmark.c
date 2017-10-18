/*
 * Test kernel, for vectorising
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <getopt.h>

#define BYTE_OPS     (1<<22)
#define WORD_OPS     (BYTE_OPS/2)
#define SINGLE_OPS   (BYTE_OPS/4)
#define DOUBLE_OPS   (BYTE_OPS/8)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef unsigned long (test_func)(void);

typedef struct {
    char        *name;
    char        *desc;
    test_func   *func;
} testdef_t;

/* Generate an array of psuedo-random data to feed into our
 * vectorisable kernel. The array needs to be nicely aligned to
 * help the vectoriser
 */
static uint8_t * get_data(uint32_t seed)
{
    unsigned long i;
    long int rseed = random();
    const int total = BYTE_OPS + 0x100;
    uint8_t *data = malloc(total * sizeof(uint8_t));

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

/* Tests */
static unsigned long bit_fiddle_bytes(void)
{
    uint8_t *add, *sub, *xor, *out;
    unsigned long i, j;

    add = align_data(get_data(0xABABCDCD));
    sub = align_data(get_data(0xDEEDBEEF));
    xor = align_data(get_data(0x7e7e7e7e));
    out = align_data(malloc((BYTE_OPS + 0x100)*sizeof(uint8_t)));

    /* Twiddle bits */
    for (j = 0; j < 100; j++) {
        for (i = 0; i < BYTE_OPS; i++)
        {
            uint8_t value = 0;
            value += add[i];
            value ^= xor[i];
            value -= sub[i];
            out[i] = value;
        }
    }

    return (BYTE_OPS * j * 3);
}

static testdef_t tests[] = {
    {
        .name = "bytewise-bit-fiddle",
        .desc = "add/xor/sub an array of bytes",
        .func = bit_fiddle_bytes
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

    fprintf(stderr, "Usage: vector-benchmark -b <testname>\n\n");
    fprintf(stderr, "Tests:\n");
    for (i = 0; i<ARRAY_SIZE(tests); i++) {
        fprintf(stderr, "%30s: %s\n", tests[i].name, tests[i].desc);
    }
}

int main(int argc, char **argv)
{
    int64_t start, end, elapsed;
    unsigned long ops;
    testdef_t *test;

    for (;;) {
        static struct option longopts[] = {
            {"help", no_argument, 0, '?'},
            {"benchmark", required_argument, 0, 'b'},
            {0, 0, 0, 0}
        };
        int optidx = 0;
        int c = getopt_long(argc, argv, "hb:", longopts, &optidx);
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

    start = get_clock();
    ops = test->func();
    end = get_clock();

    elapsed = end - start;

    fprintf(stdout, "%-20s: test took %ld msec\n", test->name, elapsed/1000);
    fprintf(stdout, "%-20s  %ld ops, ~%ld nsec/kop", "", ops, elapsed/(ops/1000));

    return 0;
}
