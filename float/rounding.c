/*
 * Test Floating Point Rounding Mode
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

static void print_number(int i, float num)
{
    int flags = fetestexcept(FE_ALL_EXCEPT);

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

    printf("%02d: %02.20e / %#010x  (%#x => %s)\n", i, num, *(uint32_t *)&num, flags, flag_state->str);
}


static void multiply_to_limit(float start)
{
    float old_num, num = start;
    int i = 0;

    printf("Doing multiply_to_limit\n");

    feclearexcept(FE_ALL_EXCEPT);
    print_number(i++, num);

    /* Cycle up until we reach our limits */
    do
    {
        old_num = num;
        num = num * M_E;
        print_number(i++, num);
    } while (isfinite(num) && !fetestexcept(FE_INVALID|FE_OVERFLOW));
}

static void divide_to_zero(float start)
{
    float old_num, num = start;
    int i = 0;

    printf("Doing divide until zero\n");

    feclearexcept(FE_ALL_EXCEPT);
    print_number(i++, num);

    /* Cycle up until we reach our limits */
    do
    {
        old_num = num;
        num = num / 2;
        print_number(i++, num);
    } while (isfinite(num) && !fetestexcept(FE_INVALID|FE_UNDERFLOW));
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
        printf("Rounding %s\n", round_flags[i].desc);

        multiply_to_limit(FLT_MIN);
        divide_to_zero(FLT_MAX);
        multiply_to_limit(-FLT_MIN);
        divide_to_zero(-FLT_MAX);
    }

    return 0;
}
