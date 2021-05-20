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
    Q->D = D;
    Q->done = false;
    Q->through_count = 0;

    return Q;
}

packet_queue_t *create_queue_pool(int num_q, int D, char L, int n)
{
    //Block allocate space for our Q structs
    packet_queue_t *Q_pool = (packet_queue_t *) malloc(num_q * sizeof(packet_queue_t));
    lock_t *L_pool = new_lock_pool(num_q, L, n);
    volatile bool *done = (volatile bool *) malloc(sizeof(volatile bool));
    *done = false;

    if (!Q_pool)
    {
        printf("ERR: failed to allocate packet_queue pool!\n");
        return NULL;
    }

    volatile Packet_t **packets = (volatile Packet_t **) malloc(num_q * D * sizeof(volatile Packet_t *));

    if (!packets)
    {
        printf("ERR: failed to allocate packet_queue\n");
        free(Q_pool);
        return NULL;
    }

    for (int i = 0; i < num_q; i++)
    {
        Q_pool[i].i = i;
        Q_pool[i].packets = packets + (i * D);
        Q_pool[i].tail = 0;
        Q_pool[i].head = 0;
        Q_pool[i].D = D;
        Q_pool[i].L = &L_pool[i];
        Q_pool[i].done = done;
        Q_pool[i].through_count = 0;
    }

    return Q_pool;
}

void clear_queue(packet_queue_t *Q)
{
    volatile Packet_t *packet;
    for(int i = 0; i < Q->D; i++)
    {
        packet = Q->packets[i];
        if (packet)
        {
            free((void*)packet);
        }
    }
    return;
}


/*MIGHT NEED TO FREE REMAINING PACKET*/
int destroy_queue_pool(int size, packet_queue_t * Q_pool)
{
    destroy_lock_pool(size, Q_pool[0].L);
    free(Q_pool[0].packets);
    free(Q_pool);
    return 0;
}


int destroy_packet_queue(packet_queue_t *Q)
{
    if (Q)
    {
        if (Q->packets)
        {
            clear_queue(Q);
            free(Q->packets);
        }
        free(Q);
    }
    return 0;
}

int enq(packet_queue_t *Q, volatile Packet_t *packet)
{
    if (Q->tail - Q->head == Q->D)
    {
        return 1;
    }

    Q->packets[Q->tail % Q->D] = packet;

    __sync_synchronize();

    Q->tail++;
    return 0;
}

volatile Packet_t *deq(packet_queue_t *Q)
{
    volatile Packet_t *packet;
    if (Q->tail - Q->head == 0)
    {
        return NULL;
    }
    packet = Q->packets[Q->head % Q->D];

    __sync_synchronize();

    Q->head++;
    return packet;
}
