/* Test behaviour of thumb instructions that cross a page boundary
 * when the second page is not mapped.
 * Copyright (c) 2015 Linaro Limited
 * License: GPL, version 2 or (at your option) any later version.
 */

#include <stdio.h>
#define __USE_GNU
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <execinfo.h>

uint8_t p[8192] __attribute__ ((aligned(4096)));

void *write_insns1(void)
{
    uint16_t *t = (uint16_t *)(p + 4088);
    void *entry = t;

    fprintf(stderr, "write_insns1: T32 insn crossing page boundary\n");

    *t++ = 0x4040;               /* 4088: EORS r0, r0, r0 */
    *t++ = 0xf200; *t++ = 0001;  /* 4090: ADDW r0, r0, #1 */
    *t++ = 0xf200; *t++ = 0001;  /* 4094: ADDW r0, r0, #1 ; spans page break */

    return entry + 1;
}

void *write_insns2(void)
{
    uint16_t *t = (uint16_t *)(p + 4088);
    void *entry = t;

    fprintf(stderr, "write_insns2: T16 insn as last in page\n");

    *t++ = 0x4040;               /* 4088: EORS r0, r0, r0 */
    *t++ = 0xf200; *t++ = 0001;  /* 4090: ADDW r0, r0, #1 */
    *t++ = 0x4040;               /* 4094: EORS r0, r0, r0 */
    *t++ = 0xf200; *t++ = 0001;  /* 4096: ADDW r0, r0, #1 ; starts on page 2 */

    return entry + 1;
}

void handler(int signum, siginfo_t *siginfo, void *ctx)
{
    ucontext_t *uctx = ctx;

    fprintf(stderr, "got sig %d\n", signum);

    fprintf(stderr, "fault pc 0x%lx r0 0x%lx\n",
            uctx->uc_mcontext.arm_pc, uctx->uc_mcontext.arm_r0);
    exit(0);
}


int main(int argc, char **argv)
{
    static struct sigaction action;
    typedef void (*entry_t)(void);
    entry_t entry;

    memset(&action, 0, sizeof(action));
    action.sa_sigaction = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART | SA_SIGINFO;;
    sigaction(SIGSEGV, &action, NULL);
    sigaction(SIGILL, &action, NULL);

    switch (argc) {
    case 1:
        entry = write_insns1();
        break;
    case 2:
        entry = write_insns2();
        break;
    default:
        fprintf(stderr, "wrong number of arguments\n");
        return 1;
    }

    if (mprotect(&p, 4096, PROT_READ|PROT_EXEC) != 0 ||
        mprotect(&p[4096], 4096, PROT_NONE) != 0) {
        fprintf(stderr, "mprotect failed\n");
        return 1;
    }

    fprintf(stderr, "Calling into buffer at %p\n", entry);
    entry();
    fprintf(stderr, "done :)\n");
    return 0;
}
