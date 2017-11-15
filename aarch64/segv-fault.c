/* segv on atomic access: check we report correct PC
 *
 * Build with:
 *  aarch64-linux-gnu-gcc -g -Wall -static -o segv-atomic segv-atomic.c
 *
 * Copyright (c) 2017 Linaro Ltd
 * License: 2-clause BSD.
 */

#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <sys/ucontext.h>
#include <stdint.h>
#include <inttypes.h>

static int sigcount = 0;
static unsigned char *mem = 0;

void sighandler(int s, siginfo_t *info, void *puc)
{
    struct ucontext *uc = puc;
    uint64_t pc = uc->uc_mcontext.pc;
    uint64_t faultaddr = uc->uc_mcontext.fault_address;

    printf("in sighandler, pc = %" PRIx64 " faultaddr %" PRIx64 "\n",
           pc, faultaddr);
    sigcount++;
    if (sigcount == 2) {
       printf("too many signals\n");
       exit(1);
    }
    /* Advance past the insn */
    uc->uc_mcontext.pc += 4;
}

int main(void)
{
    struct sigaction sa;
    int x;

    mem = mmap(0, 4096, PROT_READ | PROT_WRITE,
               MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    printf("memory at %p\n", mem);

    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sighandler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);

    memset(mem, 42, 4096);

    mprotect(mem, 4096, PROT_READ);

    /* This should fault the first time around, and then
     * the segv handler will advance the PC by 4 to skip the insn.
     * We surround it with some movs so we know the insn
     * is not at the start of a TB and so we can tell if we
     * correctly skipped it.
     */
    printf("Trying atomic stxp...\n");
    __asm__ volatile("mov %[x], #1\n"
                     "ldxp x4, x3, [%[addr]]\n"
                     "mov %[x], #2\n"
                     "stxp w5, x4, x3, [%[addr]]\n"
                     "mov %[x], #3\n"
                     : [x] "=&r" (x)
                     : [addr] "r" (mem)
                     : "w5", "x4", "x3", "memory");

    printf("...done, with x %d\n", x);
    return 0;
}
