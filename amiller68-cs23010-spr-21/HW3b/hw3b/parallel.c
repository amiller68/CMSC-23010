#include <stdio.h>
#include <stdlib.h>

#include "lib/chksum.h"

/*
 * parallel.c - generate the checksums for packets originating from N sources for M ms
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
 * D : an integer representing the depths of the queues to be used in this test
 *
 * L : a char representing the lock type to be used in our load balancing implementations. Setting this character to t, p, a, and m will direct worker threads to use either our Test and Set, the pthread Mutex, our Anderson's Array, or our MCS lock, respectively, in order to dequeue.
 *
 * S : a char representing a load balancing strategy. Setting this character to L, H, and A will direct our performance test to utilize either a LockFree, HomeQueue, or Awesome load balancing strategy, respectively.
 *
 * Returns - prints to the terminal how many packets the application processed before TO
 */
 int main(int argc, char *argv[])
 {
     //If less than 8 arguments supplied
     if (argc != 9)
     {
         printf("ERR: Inappropriate amount of arguments supplied\n");
         exit(1);
     }

     int M = atoi(argv[1]);
     int n = atoi(argv[2]);
     int W = atoi(argv[3]);
     char U = *argv[4];
     short s = atoi(argv[5]);
     int D = atoi(argv[6]);
     char L = *argv[7];
     char S = *argv[8];

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


     printf("Options intialized...\n");
     int through_count = 0;
     printf("Making packet source...\n");
     PacketSource_t *packet_source = createPacketSource(W, n, s);
     printf("Running test...\n");
     through_count = chksum_parallel(packet_source, packet_method, n, M, D, L, S);

     printf("%d\n", through_count);

     deletePacketSource(packet_source);
     return 0;
 }
