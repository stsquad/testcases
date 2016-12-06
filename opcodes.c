/*
 * Handy opcodes
 *
 * Provide a binary blob which I can disassemble with gdb
 *
 */

#include <stdio.h>
#include <stdint.h>

uint32_t isns[] = {
    0x07e00050,
    0x07a00050
};

#define N_INSNS (sizeof(isns)/sizeof(uint32_t))

void main(int c, char *v)
{
    int i;
    printf("We have %d insns\n", N_INSNS);
    for (i=0; i < N_INSNS; i++) {
        printf("  %02d: 0x%08\n", i, isns[i]);
    }
}
