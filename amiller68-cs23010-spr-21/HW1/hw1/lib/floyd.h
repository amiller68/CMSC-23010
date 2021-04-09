#ifndef FLOYD_H_
#define FLOYD_H_

#include "graph.h"

/*
 * fw_serial : performs a serial floyd-warshall operation on a graphs adjacecny
 *             matrix. This function alters the the graph's matrix in place.
 *
 * G - a pointer to a graph_t instance
 *
 * return : 0 on succes, 1 on error
 */          
int fw_serial(graph_t *G);

int fw_parallel(graph_t *G, int p);

#endif
