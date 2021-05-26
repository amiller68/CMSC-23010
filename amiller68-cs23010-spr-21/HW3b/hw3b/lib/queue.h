#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "packetsource.h"
#include "lock.h"

#include <stdbool.h>

/*A struct describing a packet queue*/
typedef struct packet_queue {
    int i;
    volatile int head, tail;
    int D;
    //A pointer to a * Packet_t array
    volatile Packet_t **packets;

    //A pointer to a flag
    volatile bool *done;

    //A non-specific shared integer for queue pools
    int N;

    lock_t *L;

    long through_count;
} packet_queue_t;

packet_queue_t *new_packet_queue(int D);

packet_queue_t *create_queue_pool(int num_q, int D, char L, int n);

int destroy_queue_pool(int size, packet_queue_t * Q_pool);

int destroy_packet_queue(packet_queue_t *Q);

long clear_queue(packet_queue_t *Q, bool correct);

int enq(packet_queue_t *Q, volatile Packet_t *packet);

volatile Packet_t *deq(packet_queue_t *Q);

#endif
