/*
 * A test output program for stressing migration
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

static const char * output_string = "The quick brown fox jumped over the lazy dog.";

static void msleep(int milliseconds)
{
    usleep(milliseconds*1000);
}

static void generate_output()
{
    char buffer[256];
    int len = strlen(output_string);
    int i, loops = (random() % (2*len));
    int pos = (random() % len);

    printf("Doing %d loops, starting with %d chars\n", loops, pos);
    for (i=0; i < loops; i++)
    {
        snprintf(buffer, sizeof(buffer), "%d/%d @ %d %.*s\n", i, loops, pos, pos, output_string);
        printf("%s", buffer);
        pos = (pos + 1) % len;
    }
}

int main(int argc, char *argv[])
{
    int fd;
    unsigned int seed;

    /* Seed the random number generator so we do vary each run */
    fd = open("/dev/urandom", 0);
    if (fd < 0 ) {
        fprintf(stderr,"%s: unable to open /dev/urandom\n", __func__);
    } else {
        read(fd, &seed, sizeof(seed));
    }
    close(fd);
    srandom(seed);

    while (1) {
        long int sleep = (random() % 2000);
        fprintf(stderr,"%s: will sleep for %ld msecs after this\n", __func__, sleep);
        generate_output();
        fprintf(stderr,"%s: sleeping...\n", __func__);
        msleep(sleep);
    }
    
    return 0;
}

