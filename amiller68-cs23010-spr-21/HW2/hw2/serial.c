#include <stdio.h>
#include <stdlib.h>

#include "lib/chksum.h"
#include "lib/stopwatch.h"

/*
 * serial.c - generate the checksums for T packets originating from N - 1 sources
 *
 * N : How many sources?
 *
 * T : How many packets per source?
 *
 * W : Average work
 *
 * D : How deep should our Lamport queues be?
 *
 * s : a seed value to pass to our generator
 *
 * -cue : what type of generator should we use?
 *
 */
int main(int argc, char *argv[])
{
    //If less than 6 arguments supplied
    if (argc != 7)
    {
        printf("ERR: Inappropriate amount of arguments supplied\n");
        exit(1);
    }

    int N = atoi(argv[1]);
    int T = atoi(argv[2]);
    int W = atoi(argv[3]);
    int D = atoi(argv[4]);
    short s = atoi(argv[5]);
    char opt = *argv[6];

    volatile Packet_t * (*packet_method)(PacketSource_t *, int);
    switch(opt)
    {
        case 'c':
            packet_method = getConstantPacket;
            break;
        case 'u':
            packet_method = getUniformPacket;
            break;
        case 'e':
            packet_method = getExponentialPacket;
            break;
        default:
            printf("ERR: Invalid generator option\n");
            return 1;
    }


    //printf("Options intialized...\n");

    StopWatch_t sw;
    long final_count = 0;
    //printf("Making packet source...\n");
    PacketSource_t *packet_source = createPacketSource(W, N - 1, s);
    //printf("Running test...\n");
    startTimer(&sw);
    final_count = chksum_serial(packet_source, packet_method, N - 1, T);
    stopTimer(&sw);
    //printf("Measuring the result...\n");
    double time = getElapsedTime(&sw);
    //printf("Writing result...\n");
    write_result(final_count, N, T, W, D, s, opt, time);
    deletePacketSource(packet_source);
    return 0;
}
