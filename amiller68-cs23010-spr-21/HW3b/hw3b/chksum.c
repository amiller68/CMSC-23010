#define _GNU_SOURCE
#include "lib/chksum.h"
#include "lib/queue.h"
#include "lib/lock.h"
#include "lib/fingerprint.h"
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>


#define UNUSED(x) (void)(x)

/* timed_flag - launch a timer in a detached threead. This thread sets the
 *              value pointed to by flag to true after M milliseconds
 *
 * volatile bool *flag : a pointer to a volatile flag
 *
 * int M : the number of milliseconds this thread should sleep for
 *
 * returns - nothing.
 */
void start_timed_flag(volatile bool *flag, int M);

long chksum_serial(PacketSource_t *packet_source, volatile Packet_t * (* packet_method)(PacketSource_t *, int), int n, int M)
{
    long through_count = 0;
    volatile Packet_t *packet;
    volatile bool *done = (volatile bool *) malloc(sizeof(bool));
    *done = false;
    int i;

    printf("Dispatching packets for %d ms...\n", M);

    i = 0;
    start_timed_flag(done, M);

    /*Prototype Dispatcher*/
    while(!*done)
    {
        packet = (*packet_method)(packet_source, i);
        through_count += getFingerprint(packet->iterations, packet->seed);
        free((void*)packet);
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
    packet_queue_t *Q_pool;
    volatile Packet_t *packet;
    pthread_t threads[N];
    void * (*worker_method)(void *);
    int i;

    switch(S)
    {
        case 'L':
            worker_method = L_worker;
            //No Locks used in this implementation
            Q_pool = create_queue_pool(N, D, 'n', 0);
            break;
        case 'H':
            worker_method = H_worker;
            //Each queue has one lock and one worker
            Q_pool = create_queue_pool(N, D, L, 1);
            break;
        case 'A':
            worker_method = A_worker;
            //No Locks used in this implementation
            Q_pool = create_queue_pool(N, D, L, N);
            break;
        default:
            printf("ERR: Invalid strategy type\n");
            return -1;
    }

    volatile bool *done = Q_pool[0].done;

    for (i = 0; i < N; i++)
    {
        pthread_create(&threads[i], NULL, worker_method, (void *) &Q_pool[i]);
    }

    printf("Dispatching packets for %d M ms...\n", M);
    i = 0;
    start_timed_flag(done, M);


    /*Prototype Dispatcher*/
    while(!*done)
    {
        packet = (*packet_method)(packet_source, i);
        while(!*done)
        {
            //While waiting, check if it's time to stop feeding....
            if(enq(&Q_pool[i], packet))
            {
                //Update i
                printf("Dispatched to queue-%d\n", i);
                i = (i + 1) % N;

                packet = NULL;
                break;
            }
        }
        if(packet)
        {
            printf("Freeing last, unused packet\n");
            //free((void*)packet);
        }
    }

    printf("Done dispatching packets!\n");

    printf("Joining threads...\n");
    for (i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
        printf("Joined : thread-%d\n", i);
        //clear_queue(&Q_pool[i]);
        printf("Cleared Queue!\n");
        through_count += Q_pool[i].through_count;
    }


    destroy_queue_pool(N, Q_pool);
    return through_count;
}

typedef struct timer_args
{
    int M;
    volatile bool *flag;
} timer_args_t;

void *timer_thread(void *arg)
{
    timer_args_t *args = (timer_args_t *) arg;
    usleep(args->M * 1000);
    *args->flag = true;
    free(args);
    return NULL;
}

void start_timed_flag(volatile bool *flag, int M)
{
    pthread_t timer;
    timer_args_t *args = (timer_args_t *) malloc(sizeof(timer_args_t));
    args->M = M;
    args->flag = flag;
    pthread_create(&timer, NULL, timer_thread, (void *) args);
    pthread_detach(timer);
    return;
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

        if (!Q->done)
        {
            getFingerprint(packet->iterations, packet->seed);
            //printf("[%ld] - Dequeued packet, res = %ld\n", pthread_self(), res);
            Q->through_count++;
            free((void*)packet);
            continue;
        }
        free((void*)packet);
        return NULL;

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
