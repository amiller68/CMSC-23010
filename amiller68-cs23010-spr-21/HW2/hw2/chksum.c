#define _GNU_SOURCE
#include "lib/chksum.h"
#include "lib/queue.h"
#include "lib/fingerprint.h"

#include <pthread.h>

long chksum_serial(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T)
{
    volatile Packet_t *packet;
    long final_count = 0;
    for (int t = 0; t < T; t++)
    {
        for (int n = 0; n < N; n++)
        {
            packet = (*packet_method)(packet_source, n);
            final_count += getFingerprint(packet->iterations, packet->seed);
        }
    }
    return final_count;
}

//perhaps adding N and T as args can be helpful
void *parallel_work_queue(void *args)
{
    packet_queue_t *Q = (packet_queue_t *) args;
    volatile Packet_t *packet;
    long res;

    while(true)
    {
        while((packet = deq(Q)) == NULL)
        {
            if (Q->done)
            {
                return NULL;
            }
            pthread_yield();
        }

        res = getFingerprint(packet->iterations, packet->seed);

        Q->final_count += res;
    }

    return NULL;
}

long chksum_parallel(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T, int D)
{
    long final_count = 0;
    packet_queue_t *Q_pool = create_queue_pool(N, D);
    volatile Packet_t *packet;
    pthread_t *threads = (pthread_t *) malloc(N * sizeof(pthread_t));
    pthread_t *thread;
    int i;

    if(!Q_pool)
    {
        return -1;
    }

    for (i = 0; i < N; i++)
    {
        thread = &threads[i];
        pthread_create(thread, NULL, &parallel_work_queue, (void *) &Q_pool[i]);
    }

    for (int t = 0; t < T; t++)
    {
        for (int n = 0; n < N; n++)
        {
            packet = (*packet_method)(packet_source, n);
            while(enq(&Q_pool[n], packet))
            {
                pthread_yield();
            }
        }
    }

    //Signal our Queue has served all the packets
    for (i = 0; i < N; i++)
    {
        Q_pool[i].done = true;
    }

    for (i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
        final_count += Q_pool[i].final_count;
    }

    return final_count;
}

typedef struct serial_queue_args {
    int N;
    int T;
    packet_queue_t *Q_pool;
} serial_queue_args_t;

void *serial_work_queue(void *args)
{
    serial_queue_args_t *thread_args = (serial_queue_args_t *) args;
    int N = thread_args->N;
    int T = thread_args->T;
    packet_queue_t *Q_pool = thread_args->Q_pool;
    volatile Packet_t *packet;
    long res;

    for (int t = 0; t < T; t++)
    {
        for (int n = 0; n < N; n++)
        {
            while((packet = deq(&Q_pool[n])) == NULL)
            {
                pthread_yield();
            }

            res = getFingerprint(packet->iterations, packet->seed);

            Q_pool[n].final_count += res;
        }
    }
    return NULL;
}

long chksum_serial_queue(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T, int D)
{
    long final_count = 0;
    packet_queue_t *Q_pool = create_queue_pool(N, D);
    volatile Packet_t *packet;
    serial_queue_args_t *args = (serial_queue_args_t *) malloc(sizeof(serial_queue_args_t));
    args->Q_pool = Q_pool;
    args->N = N;
    args->T = T;
    pthread_t thread;
    int t, n;

    if(!Q_pool)
    {
        return -1;
    }

    pthread_create(&thread, NULL, &serial_work_queue, (void *) args);

    for (t = 0; t < T; t++)
    {
        for (n = 0; n < N; n++)
        {
            packet = (*packet_method)(packet_source, n);
            while(enq(&Q_pool[n], packet))
            {
                pthread_yield();
            }
        }
    }

    pthread_join(thread, NULL);

    for (t = 0; t < N; t++)
    {
        final_count += Q_pool[t].final_count;
    }

    return final_count;
}
