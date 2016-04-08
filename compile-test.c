/*
 * A simple compile test program.
 *
 * Copyright (c) 2016
 * Written by Alex Bennée <alex@bennee.com>
 *
 * This code is licensed under the GNU GPL v3.
 */
#include <stdio.h>

int main(int argc, char *argv[])
{
    int i;
#ifdef __ATOMIC_RELAXED
    printf("Compiler has __atomic primitives.\n");
#endif

    /* Dump the arguments */
    for (i=0; i<argc; i++) {
        printf("arg%d: is %s\n", i, argv[i]);
    }
    printf("Well it runs ;-)\n");
    return 0;
}

