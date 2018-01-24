/*
 * Sizes of types and packing
 *
 * Copyright (c) 2016
 * Written by Alex Bennée <alex@bennee.com>
 *
 * This code is licensed under the GNU GPL v3.
 */
#include <stdio.h>
#include <stdbool.h>

/* Compiler barrier */
#define barrier()   ({ asm volatile("" ::: "memory"); (void)0; })

typedef struct {
    bool        one;
} one_bool_struct;

typedef struct {
    bool        one;
    bool        two;
} two_bool_struct;


int main(int argc, char *argv[])
{
    bool        standalone, yes= true, no = false;;
    one_bool_struct one_struct;
    two_bool_struct two_struct;

    fprintf(stderr,"%s: %lu bytes (x1 struct=%lu, x2 struct=%lu)\n",
            __func__, sizeof(bool), sizeof(one_struct), sizeof(two_struct));


    standalone = true;
    barrier();
    standalone = false;
    barrier();
    __atomic_store(&standalone, &yes, __ATOMIC_SEQ_CST);
    barrier();
    __atomic_store(&standalone, &no, __ATOMIC_SEQ_CST);


    return 0;
}
