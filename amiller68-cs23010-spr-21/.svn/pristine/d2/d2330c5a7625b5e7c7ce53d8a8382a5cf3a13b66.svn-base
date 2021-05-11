#define _GNU_SOURCE
#include "lib/chksum.h"
#include "lib/queue.h"
#include "lib/fingerprint.h"

#include <pthread.h>

int write_result(long count, int N, int T, long W, int D, short s, char opt, double time)
{
    /*
     file_name format:
        <n>,<T>,<W>,<s>,<time>
    */

    char file_name[100];
    char long_buff[100];
    FILE *fp;

    sprintf(file_name, "res/%d,%d,%ld,%d,%d,%c,%f", N, T, W, D, s, opt, time);

    fp = fopen(file_name, "w+");

    if(!fp)
    {
        printf("No such file: %s", file_name);
        return 1;
    }

    sprintf(long_buff, "Checksum: %ld\n", count);
    printf("Checksum: %ld\n", count);
    fputs(long_buff, fp);
    fclose(fp);
    return 0;
}

long chksum_serial(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T)
{
    volatile Packet_t *packet;
    long final_count = 0;
    long count;
    for (int t = 0; t < T; t++)
    {
        for (int n = 0; n < N; n++)
        {
            //printf("Generating packet: n = %d, t = %d\n", n, t);
            packet = (*packet_method)(packet_source, n);
            count = getFingerprint(packet->iterations, packet->seed);
            //printf("Generated chksum = %ld\n", count);
            final_count += count;
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

    //printf("[%ld] - Starting thread...\n", pthread_self());

    while(true)
    {
        //printf("[%ld] - Waiting for packet...\n", pthread_self());
        while((packet = deq(Q)) == NULL)
        {
            if (Q->done)
            {
                //printf("[%ld] - Queue is done!\n", pthread_self());
                return NULL;
            }
            pthread_yield();
        }

        res = getFingerprint(packet->iterations, packet->seed);
        //printf("[%ld] - Dequeued packet, res = %ld\n", pthread_self(), res);

        Q->final_count += res;
        free((void*)packet);
    }
}

long chksum_parallel(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int T, int D)
{
    long final_count = 0;
    packet_queue_t *Q_pool = create_queue_pool(N, D);
    volatile Packet_t *packet;
    pthread_t threads[N];
    int i;

    if(!Q_pool)
    {
        return -1;
    }

    for (i = 0; i < N; i++)
    {
        pthread_create(&threads[i], NULL, &parallel_work_queue, (void *) &Q_pool[i]);
    }

    //printf("Dispatching packets...\n");

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

    //printf("Joining threads...\n");
    for (i = 0; i < N; i++)
    {
        //printf("Joining thread-%d\n", i);
        Q_pool[i].done = true;
        //printf("Signalled done...\n");
        pthread_join(threads[i], NULL);
        //printf("Joined...\n");
        final_count += Q_pool[i].final_count;
    }

    destroy_queue_pool(Q_pool);

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
            free((void*)packet);
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

    destroy_queue_pool(Q_pool);
    return final_count;
}
