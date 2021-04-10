#include "lib/floyd.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct thread_args
{
    int i;
    int j;
    int k;
    int b_size;
    //pthread_barrier_t *sync;
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
                //Need to account for maximum edge length
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
    int num_v, b_size, i, j, k, ind;
    //pthread_t *threads, *thread;
    //pthread_barrier_t sync;

    //Allocate space to keep track of thread arguments
    args = (thread_args_t *) malloc(p * sizeof(thread_args_t));
    //Allocate space for our threads
    //threads = (pthread_t *) malloc(p * sizeof(pthread_t));

    num_v = G->num_v;
    b_size = num_v / p * 2;
    printf("Using block size: %d X %d\n", b_size, b_size);

    /*
    //Initialize a barrier
    if (pthread_barrier_init(&sync, NULL, p) != 0)
    {
        printf("ERR: Failed to create a barrier!");
        return 1;
    }
*/
    for (k = 0; k < num_v; k++, ind = 0)
    {
        for(i = 0; i < num_v; i+= b_size)
        {
            for(j = 0; j < num_v; j+= b_size, ind++)
            {
                printf("Processing: k = %d, i = %d, j = %d, ind = %d\n", k, i, j, ind);

                arg = &args[ind];
                //thread = &threads[ind];

                //Set the arguments
                arg->i = i; arg->j = j; arg->k = k; arg->b_size = b_size;
                //arg->sync = &sync;
                arg->G = G;

                update_block((void *) arg);

                //Crate a thread, thread halts until barrier is filled
                //pthread_create(thread, NULL, &update_block, (void *) arg);
            }
        }
    }

    //pthread_barrier_destroy(&sync);
    return 0;
}

void *update_block(void *args)
{
    int i, j, k, b_size, tmp;
    graph_t *G;


    thread_args_t *_args = (thread_args_t *) args;

    i = _args->i; j = _args->j; k = _args->k; b_size = _args->b_size; G = _args->G;

    printf("SQ prior:\n");
    print_sq(G, i, j, b_size);

    for(; i < _args->i + b_size; i++)
    {
        for(; j < _args->j + b_size; j++)
        {
            tmp = G->M[i][k] + G->M[k][j];
            if ((G->M[i][j] > tmp) && (tmp < MAX_EDGE))
            {
                G->M[i][j] = tmp;
            }
        }
    }
    printf("SQ after:\n");
    print_sq(G, _args->i, _args->j, b_size);

    //pthread_barrier_wait(_args->sync);
    return NULL;
}
