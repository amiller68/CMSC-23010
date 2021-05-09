#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <unistd.h>

#include "lib/lock.h"
#include "lib/stopwatch.h"

#define UNUSED(x) (void)(x)

void *sleep_worker(void *arg);
void sleeper(int B, int t);

/*
 * serial.c - count to a specified integer and print results
 *
 * B : total sleep time, in ms
 *
 * t : Critical section length, in ms
 */
int main(int argc, char *argv[])
{
    //If less than 1 arguments supplied
    if (argc < 3)
    {
        printf("ERR: Inappropriate amount of arguments supplied\n");
        exit(1);
    }

    int B = atoi(argv[1]);
    int t = atoi(argv[2]);

    StopWatch_t sw;

    startTimer(&sw);
    sleeper(B, t);
    stopTimer(&sw);

    double time = getElapsedTime(&sw);
    printf("%.*f\n", DBL_DIG-1, time);

    return 0;
}

void sleeper(int B, int t)
{
    UNUSED(B);
    UNUSED(t);
    volatile long final_count = 0;
    for(; final_count < B;)
    {
        usleep(t * 1000);
        final_count+=t;
    }
    return;
}
