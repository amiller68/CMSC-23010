#include "lib/floyd.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

typedef struct thread_args
{
    int i;
    int j;
    pthread_barrier_t *sync;
    graph_t *G;
} thread_args_t;

void *compute_block(void *args);

int fw_serial(graph_t *G)
{
    int num_v = G->num_v;
    int i, j, k, tmp;
    for(k = 0; k < num_v; k++)
    {
        for(i = 0; i < num_v; i++)
        {
            for(j = 0; j < num_v; j++)
            {
                tmp = G->M[i][k] + G->M[k][j];
                if ((G->M[i][j] > tmp) && (tmp < MAX_EDGE))
                {
                    G->M[i][j] = tmp;
                }
            }
        }
    }

    return 0;
}

int fw_parallel(graph_t *G, int p)
{
    thread_args_t *arg, *args;
    int num_v, b, i, j, ind, num_t;
    pthread_t *threads, *thread;
    pthread_barrier_t *sync = (pthread_barrier_t *) malloc(sizeof(pthread_barrier_t));

    num_v = G->num_v;
    num_t = MIN(num_v * num_v, p);

    args = (thread_args_t *) malloc(num_t * sizeof(thread_args_t));
    threads = (pthread_t *) malloc(num_t * sizeof(pthread_t));

    b  = (num_v * num_v) / num_t;

    //Maybe error check this
    if (pthread_barrier_init(sync, NULL, num_t) != 0)
    {
        printf("Failed to initialize barrier!");
    }

    for (G->b_i = 1, G->b_j = b; G->b_i < G->b_j; G->b_j = G->b_j / 2, G->b_i = G->b_i * 2){}

    //printf("Using B dimension: %d X %d\n", G->b_i, G->b_j);
    ind = 0;
    for(i = 0; i < num_v; i+= G->b_i)
    {
        for(j = 0; j < num_v; j+= G->b_j)
        {
            arg = &args[ind];
            thread = &threads[ind];

            //printf("Starting thread: i = %d , j = %d, thread = %ld\n", i, j, threads[ind]);
            arg->i = i; arg->j = j; arg->G = G; arg->sync = sync;

            pthread_create(thread, NULL, &compute_block, (void *) arg);

            ind++;
        }
    }

    for(i = 0; i < num_t; i++)
        pthread_join(threads[i], NULL);

    pthread_barrier_destroy(sync);
    return 0;
}

void *compute_block(void *args)
{
    int i, j, k, tmp;
    graph_t *G;
    thread_args_t *_args = (thread_args_t *) args;
    i = _args->i; j = _args->j; G = _args->G;

    for (k = 0; k < G->num_v; k++)
    {
        //printf("Processing sq for  k = %d: %ld\n", k, pthread_self());
        for(i = 0; i < (_args->i + G->b_i); i++)
        {
            for(j = 0; j < (_args->j + G->b_j); j++)
            {
                //printf("Processing sq for i, j, k = %d,%d,%d: %ld\n", i, j, k, pthread_self());
                tmp = G->M[i][k] + G->M[k][j];
                if ((G->M[i][j] > tmp) && (tmp < MAX_EDGE))
                {
                    G->M[i][j] = tmp;
                }
            }
        }
        //printf("Waiting for other threads: %ld\n", pthread_self());
        pthread_barrier_wait(_args->sync);
    }

    return NULL;
}
