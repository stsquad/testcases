/*
 * Check mutex reset after fork semantics.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

static pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
static __thread int the_mutex_lock_count;

void do_lock(void)
{
    if (the_mutex_lock_count++ == 0) {
        pthread_mutex_lock(&the_mutex);
    }
}

void do_unlock_or_reset(int child)
{
    if (child)
        pthread_mutex_init(&the_mutex, NULL);
    else
        pthread_mutex_unlock(&the_mutex);
}

int main(void)
{
    int ret;
    do_lock();
    ret = fork();
    if (ret == 0) {
        do_unlock_or_reset(1);
    } else {
        do_unlock_or_reset(0);
    }
    fprintf(stderr, "Exiting with errno:%d/%s\n", errno, strerror(errno));
}
