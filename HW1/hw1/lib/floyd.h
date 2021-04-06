#ifndef FLOYD_H_
#define FLOYD_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct graph {
    int V;
    int E;
} graph_t;

graph_t *construct_graph(FILE *specs);

int fw_sequential(graph_t *g);

int fw_parallel(graph_t *g);

#endif
