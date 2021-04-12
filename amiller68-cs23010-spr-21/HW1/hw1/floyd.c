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
    graph_t *G;
} thread_args_t;

void *update_block(void *args);

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

    num_v = G->num_v;
    num_t = MIN(num_v * num_v, p);

    args = (thread_args_t *) malloc(num_t * sizeof(thread_args_t));
    threads = (pthread_t *) malloc(num_t * sizeof(pthread_t));

    b  = (num_v * num_v) / num_t;

    for (G->b_i = 1, G->b_j = b; G->b_i < G->b_j; G->b_j = G->b_j / 2, G->b_i = G->b_i * 2){}

    for (G->k = 0; G->k < num_v; G->k++, ind = 0)
    {
        for(i = 0; i < num_v; i+= G->b_i)
        {
            for(j = 0; j < num_v; j+= G->b_j, ind++)
            {
                arg = &args[ind];
                thread = &threads[ind];

                arg->i = i; arg->j = j; arg->G = G;

                pthread_create(thread, NULL, &update_block, (void *) arg);
            }
        }

        for (i = 0; i < num_t; i++)
            pthread_join(threads[i], NULL);
    }

    return 0;
}

void *update_block(void *args)
{
    int i, j, tmp;
    graph_t *G;
    thread_args_t *_args = (thread_args_t *) args;
    i = _args->i; j = _args->j; G = _args->G;

    for(; i < (_args->i + G->b_i); i++)
    {
        for(j = 0; j < (_args->j + G->b_j); j++)
        {
            tmp = G->M[i][G->k] + G->M[G->k][j];
            if ((G->M[i][j] > tmp) && (tmp < MAX_EDGE))
            {
                G->M[i][j] = tmp;
            }
        }
    }

    return NULL;
}
