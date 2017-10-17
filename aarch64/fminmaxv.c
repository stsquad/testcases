/* gcc-7 -march=armv8.2-a+fp16 fminmaxv.c -o fminmaxv */
#include <stdio.h>
#include <arm_neon.h>
#include <inttypes.h>

int main(void)
{
    uint16x8_t input_hex = {0x1231, 0xd899, 0xf6c4, 0x3a4a, 0xf9ac, 0xc201, 0x7d49, 0x55f0};
    uint16x8_t input_hex_2 = {0xedff, 0x26b6, 0x7e3a, 0x602b, 0x8b26, 0x7c09, 0x3d64, 0x40bd};
    uint16_t result;

    asm("fmaxnmv %h0,%1.8h" : "=w" (result) : "x" (input_hex));
    printf("fmaxnmv asm result=%#" PRIx16"\n", result);

    asm("fminv %h0,%1.8h" : "=w" (result) : "x" (input_hex_2));
    printf("fminv asm result=%#" PRIx16"\n", result);

    return 0;
}
