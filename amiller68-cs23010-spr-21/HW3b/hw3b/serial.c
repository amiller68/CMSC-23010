#include <stdio.h>
#include <stdlib.h>

#include "lib/chksum.h"
#include "lib/stopwatch.h"

/*
 * serial.c - generate the checksums for packets originating from N sources for M ms
 *
 * M :  an integer representing the time in milliseconds that the experiment should run.
 *
 * n :  an integer representing the total number of sources
 *
 * W : n integer representing the expected amount of work per packet
 *
 * U :  an char representing a flag specifying what sort of packet distribution to utilize in our tests. $t$ indicates the use of Uniformly Distributed Packets , while $f$ Exponentially Distributed Packets.
 *
 * s :  a non-negative integer, for seeding the packet generator. In practice this will correspond to trial numbers when testing is run.
 *
 * Returns - prints to the terminal how many packets the application processed before TO
 */
int main(int argc, char *argv[])
{
    //If less than 5 arguments supplied
    if (argc != 6)
    {
        printf("ERR: Inappropriate amount of arguments supplied\n");
        exit(1);
    }

    int M = atoi(argv[1]);
    int n = atoi(argv[2]);
    int W = atoi(argv[3]);
    char U = *argv[4];
    short s = atoi(argv[5]);

    volatile Packet_t * (*packet_method)(PacketSource_t *, int);
    switch(U)
    {
        case 't':
            packet_method = getUniformPacket;
            break;
        case 'f':
            packet_method = getExponentialPacket;
            break;
        default:
            printf("ERR: Invalid generator option\n");
            return 1;
    }


    //printf("Options intialized...\n");
    long through_count = 0;
    //printf("Making packet source...\n");
    PacketSource_t *packet_source = createPacketSource(W, n, s);
    //printf("Running test...\n");
    through_count = chksum_serial(packet_source, packet_method, n, M);

    printf("%ld \n", through_count);

    deletePacketSource(packet_source);
    return 0;
}
