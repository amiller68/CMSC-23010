#include <stdio.h>
#include <stdlib.h>

#include "lib/chksum.h"
#include "lib/stopwatch.h"

int write_result(long count, int N, int T, long W, short s, double time);

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
    //int D = atoi(argv[4]);
    short s = atol(argv[5]);
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

    StopWatch_t sw;

    PacketSource_t *packet_source = createPacketSource(W, N - 1, s);

    startTimer(&sw);
    long final_count = chksum_serial(packet_source, packet_method, N - 1, T);
    stopTimer(&sw);

    double time = getElapsedTime(&sw);

    write_result(final_count, N, T, W, s, time);
    deletePacketSource(packet_source);
    return 0;
}

int write_result(long count, int N, int T, long W, short s, double time)
{
    /*
     file_name format:
        <n>,<T>,<W>,<s>,<time>
    */

    char file_name[100];
    char long_buff[20];
    FILE *fp;

    sprintf(file_name, "res/%d,%d,%ld,%d,%f", N, T, W, s, time);

    fp = fopen(file_name, "w+");

    if(!fp)
    {
        printf("No such file: %s", file_name);
        return 1;
    }

    sprintf(long_buff, "Checksum: %ld\n", count);
    fputs(long_buff, fp);
    fclose(fp);
    return 0;
}
