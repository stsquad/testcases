/*
 * Test Floating Point Conversion
 */

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <fenv.h>

#include <glib.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

GString *flag_state;

/* Print human readable flags */
static GString * append_flag(GString *src, char *flg)
{
    if (src->len) {
        src = g_string_append(src, " ");
    }
    return g_string_append(src, flg);
}

static char * get_flag_state(int flags)
{
    flag_state = g_string_truncate(flag_state, 0);

    if (flags) {
        if (flags & FE_OVERFLOW) {
            flag_state = append_flag(flag_state, "OVERFLOW");
        }
        if (flags & FE_UNDERFLOW) {
            flag_state = append_flag(flag_state, "UNDERFLOW");
        }
        if (flags & FE_DIVBYZERO) {
            flag_state = append_flag(flag_state, "DIV0");
        }
        if (flags & FE_INEXACT) {
            flag_state = append_flag(flag_state, "INEXACT");
        }
        if (flags & FE_INVALID) {
            flag_state = append_flag(flag_state, "INVALID");
        }
    } else {
        flag_state = append_flag(flag_state, "OK");
    }

    return flag_state->str;
}

static void print_double_number(int i, double num)
{
    uint64_t double_as_hex = *(uint64_t *) &num;
    int flags = fetestexcept(FE_ALL_EXCEPT);
    char *fstr = get_flag_state(flags);

    printf("%02d DOUBLE: %02.20e / %#020x  (%#x => %s)\n",
           i, num, double_as_hex, flags, fstr);
}

static void print_single_number(int i, float num)
{
    uint32_t single_as_hex = *(uint32_t *) &num;
    int flags = fetestexcept(FE_ALL_EXCEPT);
    char *fstr = get_flag_state(flags);

    printf("%02d SINGLE: %02.20e / %#010x  (%#x => %s)\n",
           i, num, single_as_hex, flags, fstr);
}

static void print_half_number(int i, uint16_t num)
{
    int flags = fetestexcept(FE_ALL_EXCEPT);
    char *fstr = get_flag_state(flags);

    printf("%02d   HALF: %#010x  (%#x => %s)\n",
           i, num, flags, fstr);
}

float single_numbers[] = { -FLT_MAX, -FLT_MIN,
                           0.0,
                           FLT_MIN,
                           1.0, 2.0,
                           M_E, M_PI,
                           0x9EA82A22,
                           0xAB98FBA8,
                           FLT_MAX };

static void convert_single_to_half(void)
{
    int i;

    printf("Converting single-precision to half-precision\n");

    for (i = 0; i < ARRAY_SIZE(single_numbers); ++i) {
        float input = single_numbers[i];
        uint16_t output;

        feclearexcept(FE_ALL_EXCEPT);

        print_single_number(i, input);
#ifdef __aarch64__
        asm("fcvt %h0, %d1" : "=w" (output) : "x" (input));
#else
        /* need compiler with _Float16 support */
        output = 0;
#endif
        print_half_number(i, output);
    }
}

double double_numbers[] = { -DBL_MAX, -DBL_MIN,
                            0.0,
                            DBL_MIN,
                            1.0, 2.0,
                            M_E, M_PI,
                            0x9EA82A2287680UL,
                            0xAB98FBA843210UL,
                            DBL_MAX };

static void convert_double_to_half(void)
{
    int i;

    printf("Converting double-precision to half-precision\n");

    for (i = 0; i < ARRAY_SIZE(single_numbers); ++i) {
        double input = double_numbers[i];
        uint16_t output;

        feclearexcept(FE_ALL_EXCEPT);

        print_double_number(i, input);
#ifdef __aarch64__
        asm("fcvt %h0, %s1" : "=w" (output) : "x" (input));
#else
        /* need compiler with _Float16 support */
        output = 0;
#endif
        print_half_number(i, output);
    }
}

typedef struct {
    int flag;
    char *desc;
} float_mapping;

float_mapping round_flags[] =
{
    { FE_TONEAREST, "to nearest" },
    { FE_UPWARD, "upwards" },
    { FE_DOWNWARD, "downwards" },
    { FE_TOWARDZERO, "to zero" }
};

int main(int argc, char *argv[argc])
{
    int i;
    flag_state = g_string_new(NULL);

    for (i = 0; i < ARRAY_SIZE(round_flags); ++i) {
        fesetround(round_flags[i].flag);
        printf("### Rounding %s\n", round_flags[i].desc);
        convert_single_to_half();
        convert_double_to_half();
    }

    /* And now with ARM alternative FP16 */
    asm("msr fpsr, x1\n\t"
        "orr x1, x1, %[flags]\n\t"
        "mrs x1, fpsr\n\t"
        : /* no output */ : [flags] "n" (1 << 26) : "x1" );

    printf("#### Enabling ARM Alternative Half Precision\n");

    for (i = 0; i < ARRAY_SIZE(round_flags); ++i) {
        fesetround(round_flags[i].flag);
        printf("### Rounding %s\n", round_flags[i].desc);
        convert_single_to_half();
        convert_double_to_half();
    }

    return 0;
}
