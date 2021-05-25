#define _GNU_SOURCE
#include "lib/chksum.h"
#include "lib/queue.h"
//#include "lib/lock.h"
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

    //printf("Dispatching packets for %d ms...\n", M);

    i = 0;
    start_timed_flag(done, M);

    /*Prototype Dispatcher*/
    while(!*done)
    {
        packet = (*packet_method)(packet_source, i);
        getFingerprint(packet->iterations, packet->seed);
        through_count++;
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
            //Each queue has one lock with N potential workers
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

    //printf("Dispatching packets for %d M ms...\n", M);
    i = 0;
    start_timed_flag(done, M);


    /*Dispatcher*/

    while(!*done)
    {
        packet = (*packet_method)(packet_source, i);
        //printf("Enqueuing packet_id = %p in Queue-%d...\n", packet, i);
        while(enq(&Q_pool[i], packet))
        {
            if(*done)
            {
                //printf("Freeing last, unused packet\n");
                free((void*)packet);
                break;
            }
            pthread_yield();
        }
        i = (i + 1) % N;
    }

    //printf("Done dispatching packets!\n");

    //printf("Joining threads...\n");
    for (i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
        //printf("Joined : thread-%d\n", i);
        clear_queue(&Q_pool[i]);
        //printf("Cleared Queue!\n");

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
    //printf("Timer flipped the flag!\n");
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
    packet_queue_t *Q = (packet_queue_t *) args;
    volatile bool *done = Q->done;
    volatile Packet_t *packet;
    //printf("[%ld] - Starting thread...\n", pthread_self());
    //printf("Started L_worker: thread-%ld\n", pthread_self());
    while(true)
    {
        //printf("[%ld] - Preparing to dequeu...\n", pthread_self());
        while((packet = deq(Q)) == NULL)
        {
            //printf("Failed to dequeu: thread-%ld\n", pthread_self());
            if (*done)
            {
                //printf("[%ld] - Queue is done!\n", pthread_self());
                return NULL;
            }
            pthread_yield();
        }
        //printf("[%ld] - Succesfully dequeued\n", pthread_self());

        //printf("[%ld] - Operating on packet: packet_id = %p\n", pthread_self(), packet);
        getFingerprint(packet->iterations, packet->seed);
        free((void*)packet);

        if (*done)
        {
            //printf("[%ld] - Succesfully operated on packet, but not on time!\n", pthread_self());
            return NULL;
        }

        Q->through_count++;
    }
    return NULL;
}

//perhaps adding N and T as args can be helpful
void *H_worker(void *args)
{
    packet_queue_t *Q = (packet_queue_t *) args;
    volatile bool *done = Q->done;
    volatile Packet_t *packet;
    lock_t *L = Q->L;
    //printf("[%ld] - Starting thread...\n", pthread_self());
    //printf("Started L_worker: thread-%ld\n", pthread_self());
    while(true)
    {
        //printf("[%ld] - Preparing to dequeu...\n", pthread_self());
        L->lock(L->l);

        while((packet = deq(Q)) == NULL)
        {

            //printf("Failed to dequeu: thread-%ld\n", pthread_self());
            if (*done)
            {
                L->unlock(L->l);

                //printf("[%ld] - Queue is done!\n", pthread_self());
                return NULL;
            }
            pthread_yield();
        }
        L->unlock(L->l);

        //printf("[%ld] - Succesfully dequeued\n", pthread_self());

        //printf("[%ld] - Operating on packet: packet_id = %p\n", pthread_self(), packet);
        getFingerprint(packet->iterations, packet->seed);
        free((void*)packet);

        if (*done)
        {
            //printf("[%ld] - Succesfully operated on packet, but not on time!\n", pthread_self());
            return NULL;
        }

        Q->through_count++;
    }
    return NULL;
}

//perhaps adding N and T as args can be helpful
void *A_worker(void *args)
{
    packet_queue_t *Q = (packet_queue_t *) args;
    volatile bool *done = Q->done; // a pointer to a shared flag
    int N = *Q->N; //How many queues are in this pool?
    int i = Q->i; //The threads initial index into the queue_pool
    packet_queue_t *Q_pool = Q - i; //Get the pointer to start of the queue_pool
    volatile Packet_t *packet;
    lock_t *L = Q->L; //Initial lock pointer
    //printf("[%ld] - Starting A_worker...\n", pthread_self());

    while(true)
    {
        //printf("[%ld] - Preparing to trylock-%d...\n", pthread_self(), i);
        if(L->trylock(L->l))
        {
            //printf("[%ld] - Acquired lock-%d\n", pthread_self(), i);
            //If the worker failed to dequeue...
            packet = deq(Q);
            L->unlock(L->l);

            if (packet)
            {
                //printf("[%ld] - Got a packet!\n", pthread_self());
                getFingerprint(packet->iterations, packet->seed);
                free((void*)packet);
            }

            if (*done)
            {
                //printf("[%ld] - Succesfully operated on packet, but not on time!\n", pthread_self());
                return NULL;
            }
            __sync_fetch_and_add(&Q->through_count, 1);
        }

        if (*done)
        {
            //printf("[%ld] - Succesfully operated on packet, but not on time!\n", pthread_self());
            return NULL;
        }

        /*Move on to the next Queue*/
        i = (i + 1) % N; //Update i
        Q = &Q_pool[i];   //Update Q
        L = Q->L;        //Update L
    }
    return NULL;
}
