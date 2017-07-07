#include <asm/hwcap.h>
#include <stdio.h>
#include <sys/auxv.h>

#define get_cpu_ftr(id) ({                                      \
                unsigned long __val;                            \
                asm("mrs %0, "#id : "=r" (__val));              \
                printf("%-20s: 0x%016lx\n", #id, __val);        \
        })

int main(void)
{

        if (!(getauxval(AT_HWCAP) & HWCAP_CPUID)) {
                fputs("CPUID registers unavailable\n", stderr);
                return 1;
        }

        get_cpu_ftr(ID_AA64ISAR0_EL1);
        get_cpu_ftr(ID_AA64ISAR1_EL1);
        get_cpu_ftr(ID_AA64MMFR0_EL1);
        get_cpu_ftr(ID_AA64MMFR1_EL1);
        get_cpu_ftr(ID_AA64PFR0_EL1);
        get_cpu_ftr(ID_AA64PFR1_EL1);
        get_cpu_ftr(ID_AA64DFR0_EL1);
        get_cpu_ftr(ID_AA64DFR1_EL1);

        get_cpu_ftr(MIDR_EL1);
        get_cpu_ftr(MPIDR_EL1);
        get_cpu_ftr(REVIDR_EL1);

#if 0
        /* Unexposed register access causes SIGILL */
        get_cpu_ftr(ID_MMFR0_EL1);
#endif

        return 0;
}
