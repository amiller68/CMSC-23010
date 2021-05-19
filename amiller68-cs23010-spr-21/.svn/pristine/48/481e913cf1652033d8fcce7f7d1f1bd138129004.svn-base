#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <unistd.h>

#include "lib/lock.h"
#include "lib/stopwatch.h"

#define UNUSED(x) (void)(x)

void sleeper(int B, int t, int n, lock_t * L);

/*
 * serial.c - count to a specified integer and print results
 *
 * B : total sleep time, in ms
 *
 * t : Critical section length, in ms
 *
 * n : a number of threads to use (not used in this file)
 *
 * L : a char specifying a Locking algorithm (not used in this file)
 *     t, p, a, and m will direct threads to use our Test and Set Lock,
 *     the pthread Mutex, our Anderson’s Array Lock, and our MCS lock, respectively.
 *
 */
int main(int argc, char *argv[])
{
    //If less than 1 arguments supplied
    if (argc < 5)
    {
        printf("ERR: Inappropriate amount of arguments supplied\n");
        exit(1);
    }

    int B = atoi(argv[1]);
    int t = atoi(argv[2]);
    int n = atoi(argv[3]);
    char type = *argv[4];

    lock_t *L = new_lock(type, n);
    L->B = B;
    L->t = t;

    StopWatch_t sw;

    startTimer(&sw);
    sleeper(B, t, n, L);
    stopTimer(&sw);

    double time = getElapsedTime(&sw);
    printf("%.*f \n", DBL_DIG-1, time);

    destroy_lock(L);
    return 0;
}

void *sleep_worker(void *arg)
{
    lock_t *L = (lock_t *) arg;
    L->init_thread(L->l);
    while(1)
    {
        L->lock(L->l);
        if (L->counter < L->B)
        {
            usleep(L->t * 1000);
            L->counter += L->t;
            //printf("Counter = %ld\n", L->counter);
            L->unlock(L->l);
            continue;
        }
        L->unlock(L->l);
        break;
    }
    return NULL;
}

void sleeper(int B, int t, int n, lock_t * L)
{
    int i;
    pthread_t threads[n];
    pthread_attr_t tattr;

/*
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
*/
    for(i = 0; i < n - 1; i++)
    {
        //pthread_create(&threads[i], &tattr, &count_worker, (void *) L);
        pthread_create(&threads[i], NULL, &sleep_worker, (void *) L);
    }

    L->init_thread(L->l);

    while(1)
    {
        L->lock(L->l);
        if (L->counter < B)
        {
            usleep(t * 1000);
            L->counter += t;
            //printf("Counter = %ld\n", L->counter);
            L->unlock(L->l);
            continue;
        }
        L->unlock(L->l);
        break;
    }

    for(i = 0; i < n - 1; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return;
}
