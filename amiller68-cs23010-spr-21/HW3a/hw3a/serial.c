#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "lib/lock.h"
#include "lib/stopwatch.h"

long counter(int B);

/*
 * serial.c - count to a specified integer and print results
 *
 * B : a number to count to
 */
int main(int argc, char *argv[])
{
    //If less than 1 arguments supplied
    if (argc < 2)
    {
        printf("ERR: Inappropriate amount of arguments supplied\n");
        exit(1);
    }

    int B = atoi(argv[1]);

    StopWatch_t sw;
    volatile long final_count;

    startTimer(&sw);
    final_count = counter(B);
    stopTimer(&sw);

    double time = getElapsedTime(&sw);
    printf("%ld,%.*f\n", final_count, DBL_DIG-1, time);
    return 0;
}

long counter(int B)
{
    volatile long final_count = 0;
    for(; final_count < B;)
    {
        //printf("eh\n");
        final_count++;
    }
    return final_count;
}
