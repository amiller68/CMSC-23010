#ifndef FLOYD_H_
#define FLOYD_H_

#include "graph.h"

int fw_serial(graph_t *G);

int fw_parallel(graph_t *G, int p);

#endif
