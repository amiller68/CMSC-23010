#define _GNU_SOURCE
#include "lib/chksum.h"
#include "lib/queue.h"
#include "lib/lock.h"
#include "lib/fingerprint.h"
#include <time.h>
#include <pthread.h>

#define UNUSED(x) (void)(x)

long chksum_serial(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int n, int M)
{
    long through_count = 0;
    volatile Packet_t *packet;
    double runtime = (double) M / 1000;
    double elapsedTime = 0.0;
    time_t start, now;
    int i;

    printf("Dispatching packets for %f s...\n", runtime);

    i = 0;
    time(&start);

    /*Prototype Dispatcher*/
    while(elapsedTime < runtime)
    {
        packet = (*packet_method)(packet_source, i);
        through_count += getFingerprint(packet->iterations, packet->seed);
        free((void*)packet);

        now = time(NULL);
        elapsedTime = difftime(now, start);
        i = (i + 1) % n;
    }

    return through_count;
}

/*Forward Declaration of concurrent methods and data structures*/
void *L_worker(void *args);
void *H_worker(void *args);
void *A_worker(void *args);

typedef struct thread_args {
    int i;
    packet_queue_t *Q;
    lock_t *L;
} thread_args_t;

long chksum_parallel(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int N, int M, int D, char L, char S)
{
    long through_count = 0;
    packet_queue_t *Q_pool = create_queue_pool(N, D);
    volatile Packet_t *packet;
    pthread_t threads[N];
    thread_args_t *T = (thread_args_t *) calloc(N, sizeof(thread_args_t));
    void * (*worker_method)(void *);
    lock_t *L_pool;
    double runtime = (double) M / 1000;
    double elapsedTime = 0.0;
    time_t start, now;
    int i;

    if(!Q_pool)
    {
        return -1;
    }

    switch(S)
    {
        case 'L':
            worker_method = L_worker;
            //No Locks used in this implementation
            L_pool = NULL;
            break;
        case 'H':
            worker_method = H_worker;
            //Each queue has one lock and one worker
            L_pool = new_lock_pool(N, L, 1);
            break;
        case 'A':
            worker_method = A_worker;
            //No Locks used in this implementation
            L_pool = NULL;
            break;
        default:
            printf("ERR: Invalid strategy type\n");
            destroy_queue_pool(Q_pool);
            return -1;
    }

    for (i = 0; i < N; i++)
    {
        T[i].i = i;
        T[i].Q = &Q_pool[i];
        T[i].L = &L_pool[i];
        pthread_create(&threads[i], NULL, worker_method, (void *) &T[i]);
    }

    printf("Dispatching packets for %f s...\n", runtime);
    i = 0;
    time(&start);

    /*
    while(1)
    {
        packet = (*packet_method)(packet_source, i);
        while(elapsedTime < runtime)
        {
            if(enq(&Q_pool[i], packet))
            {
                i = (i + 1) % N;
                break;
            }

            now = time(NULL);
            elapsedTime = difftime(now, start);
        }
    }
    */

    /*Prototype Dispatcher*/
    while(elapsedTime < runtime)
    {
        packet = (*packet_method)(packet_source, i);
        while(elapsedTime < runtime)
        {
            //While waiting, check if it's time to stop feeding....
            if(enq(&Q_pool[i], packet))
            {
                //Update i
                printf("Dispatched to queue-%d\n", i);
                i = (i + 1) % N;
                //Get New time diff
                now = time(NULL);
                elapsedTime = difftime(now, start);
                packet = NULL;
                break;
            }

            now = time(NULL);
            elapsedTime = difftime(now, start);
        }
        if(packet)
        {
            printf("Freeing last, unused packet\n");
            //free((void*)packet);
        }
    }

    printf("Done dispatching packets!\n");

    for (i = 0; i < N; i++)
    {
        printf("Signalling done: thread-%d\n", i);
        Q_pool[i].done = true;
    }

    printf("Joining threads...\n");
    for (i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
        printf("Joined : thread-%d\n", i);
        //clear_queue(&Q_pool[i]);
        printf("Cleared Queue!\n");
        through_count += Q_pool[i].through_count;
    }

    if(L_pool)
    {
        destroy_lock_pool(N, L_pool);
    }

    destroy_queue_pool(Q_pool);
    return through_count;
}

//perhaps adding N and T as args can be helpful
void *L_worker(void *args)
{
    thread_args_t *T = (thread_args_t *) args;
    packet_queue_t *Q = T->Q;
    volatile Packet_t *packet;
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

        getFingerprint(packet->iterations, packet->seed);
        //printf("[%ld] - Dequeued packet, res = %ld\n", pthread_self(), res);

        Q->through_count ++;
        free((void*)packet);
    }
    return NULL;
}

//perhaps adding N and T as args can be helpful
void *H_worker(void *args)
{
    thread_args_t *T = (thread_args_t *) args;
    packet_queue_t *Q = T->Q;
    volatile Packet_t *packet;
    //printf("[%ld] - Starting thread...\n", pthread_self());

    while(!Q->done)
    {
        //printf("[%ld] - Waiting for packet...\n", pthread_self());
        while((packet = deq(Q)) == NULL)
        {
            pthread_yield();
        }

        getFingerprint(packet->iterations, packet->seed);
        //printf("[%ld] - Dequeued packet, res = %ld\n", pthread_self(), res);

        Q->through_count++;
        free((void*)packet);
    }
    return NULL;
}

//perhaps adding N and T as args can be helpful
void *A_worker(void *args)
{
    thread_args_t *T = (thread_args_t *) args;
    packet_queue_t *Q = T->Q;
    volatile Packet_t *packet;
    //printf("[%ld] - Starting thread...\n", pthread_self());

    while(!Q->done)
    {
        //printf("[%ld] - Waiting for packet...\n", pthread_self());
        while((packet = deq(Q)) == NULL)
        {
            pthread_yield();
        }

        getFingerprint(packet->iterations, packet->seed);
        //printf("[%ld] - Dequeued packet, res = %ld\n", pthread_self(), res);

        Q->through_count++;
        free((void*)packet);
    }
    return NULL;
}
