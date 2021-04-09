#include <stdio.h>
#include <stdlib.h>

#include "lib/floyd.h"
#include "lib/graph.h"
#include "lib/stopwatch.h"

/*
 * Takes as input:
 *
 * file_name - a path to a file to read in as input
 *
 * p - how many threads should the parallel implementation utilize
 */
 int main(int argc, char *argv[])
 {
     //If less than two arguments supplied
     if (argc != 3)
     {
         printf("Inappropriate amount of arguments supplied\n");
         exit(1);
     }

     char *prog_name = argv[0];
     char *file_name = argv[1];
     int p = atoi(argv[2]);
     StopWatch_t sw;

     /*constructs a graph from a file*/
     graph_t *G = construct_graph(file_name);

     //printf("Loaded Graph:\n");
     //print_graph(G);

     startTimer(&sw);
     fw_parallel(G, p);
     stopTimer(&sw);

     double time = getElapsedTime(&sw);

     //printf("Computed Result:\n");
     //print_graph(G);

     write_result(G->num_v, p, prog_name, time, G);
     return 0;
 }
