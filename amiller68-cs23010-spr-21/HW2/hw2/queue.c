#include "lib/queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

packet_queue_t *new_packet_queue(int D)
{
    //Allocate space for a packet_queue
    packet_queue_t *Q = (packet_queue_t *) malloc(sizeof(packet_queue_t));

    if (!Q)
    {
        printf("ERR: failed to allocate packet_queue\n");
        return NULL;
    }

    //Allocate space for our packet queue
    volatile Packet_t **packets = (volatile Packet_t **) malloc(D * sizeof(Packet_t));

    if (!packets)
    {
        printf("ERR: failed to allocate packet_queue\n");
        free(Q);
        return NULL;
    }

    Q->packets = packets;
    Q->tail = 0;
    Q->head = 0;
    Q->done = false;
    Q->final_count = 0;

    return Q;
}

packet_queue_t *create_queue_pool(int num_q, int D)
{
    //Block allocate space for our Q structs
    packet_queue_t *Q_pool = (packet_queue_t *) malloc(num_q * sizeof(packet_queue_t));

    if (!Q_pool)
    {
        printf("ERR: failed to allocate packet_queue pool!\n");
        return NULL;
    }

    //Block allocate space for our packet queues
    //I want num_q arrays of size D to Packet_t pointers
    volatile Packet_t **packets = (volatile Packet_t **) malloc(num_q * D * sizeof(volatile Packet_t *));

    if (!packets)
    {
        printf("ERR: failed to allocate packet_queue\n");
        free(Q_pool);
        return NULL;
    }

    for (int i = 0; i < num_q; i++)
    {
        Q_pool[i].packets = packets + (i * D);
        Q_pool[i].tail = 0;
        Q_pool[i].head = 0;
        Q_pool[i].done = false;
        Q_pool[i].final_count = 0;
    }

    return Q_pool;
}

int destroy_packet_queue(packet_queue_t *Q)
{
    if (Q)
    {
        if (Q->packets)
        {
            free(Q->packets);
        }
        free(Q);
    }
    return 0;
}

int enq(packet_queue_t *Q, volatile Packet_t *packet)
{
    if (Q->tail - Q->head == sizeof(Q->packets))
    {
        return 1;
    }

    Q->packets[Q->tail % sizeof(Q->packets)] = packet;
    Q->tail++;
    return 0;
}

volatile Packet_t *deq(packet_queue_t *Q)
{
    if (Q->tail - Q->head == 0)
    {
        return NULL;
    }

    Q->head++;
    return Q->packets[Q->head % sizeof(Q->packets)];
}
