/* gcc-7 -march=armv8.2-a+fp16 fmax.c -o fmax */
#include <stdio.h>
#include <arm_neon.h>
#include <inttypes.h>

int main(void)
{
    uint16x8_t input_hex = {0x1231, 0xd899, 0xf6c4, 0x3a4a, 0xf9ac, 0xc201, 0x7d49, 0x55f0};
    uint16_t result;

    asm("fmaxnmv %h0,%0.8h" : "+w" (result) : "x" (input_hex));
    printf("from asm result=%#" PRIx16"\n", result);

    asm("fmaxnmv %h0,%1.8h" : "=w" (result) : "x" (input_hex));
    printf("from asm result=%#" PRIx16"\n", result);

    return 0;
}
