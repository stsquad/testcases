/*
 * Unsigned arithmetic
 *
 * Copyright (c) 2017
 * Written by Alex Bennée <alex@bennee.com>
 *
 * This code is licensed under the GNU GPL v3.
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

int main(int argc, char *argv[])
{
    int i;
    unsigned int ui = 5;
    unsigned long ul = 5;
    uint16_t u16 = 5;
    uint32_t u32 = 5;
    uint64_t u64 = 5;

    fprintf(stderr, "unsigned underflow\n");
    for (i = 0; i < 10; i++) {
        ui--; ul--; u16--; u32--; u64--;

        fprintf(stderr,
                "%d: ui:%u/%#x ul:%ld/%#lx "
                "u16:%#" PRIx16 " u32:%#" PRIx32 " u64:%#" PRIx64 "\n",
                i, ui, ui, ul, ul, u16, u32, u64);
    }

    ui = UINT_MAX - 5;
    ul = ULONG_MAX - 5;
    u16 = UINT16_MAX - 5;
    u32 = UINT32_MAX - 5;
    u64 = UINT64_MAX - 5;

    fprintf(stderr, "unsigned overflow\n");

    for (i = 0; i < 10; i++) {
        ui++; ul++; u16++; u32++; u64++;

        fprintf(stderr,
                "%d: ui:%u/%#x ul:%ld/%#lx "
                "u16:%#" PRIx16 " u32:%#" PRIx32 " u64:%#" PRIx64 "\n",
                i, ui, ui, ul, ul, u16, u32, u64);
    }

    return 0;
}
