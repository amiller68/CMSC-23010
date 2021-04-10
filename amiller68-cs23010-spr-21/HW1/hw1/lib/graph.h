#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdbool.h>

#define MAX_V 1024
#define MAX_EDGE 10
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct graph {
    int num_v; //NUmber of vertices in the graph
    int M[MAX_V][MAX_V]; //Easiest to implement
} graph_t;


/*
 * construct_graph : constructs a graph representation from an appropriately
 *                   formatted text file. Does not take malformed text files as input,
 *                   assumes a file describes an N X N adjacecny matrix. Assumes
 *                   that files are named "n.txt"
 *
 * file_name : a pointer to a char array containing a file name to read
 *
 * returns : a pointer to a newly created graph_t instance
 */
graph_t *construct_graph(char *file_name);

/*
 * print_graph : prints the contents of an adjacecny matrix
 *
 * G - a pointer to a graph_t instance
 *
 * returns : nothing
 */
void print_graph(graph_t *G);

void print_sq(graph_t *G, int i, int j, int b_size);

/*
 * write_result : writes the contents of an adjacecny matrix to an appropriately
 *                named text file
 *
 * n - number of vertices to include
 *
 * t - how many threads were used
 *
 * prog - a char pointer to the program name
 *
 * time - a double describing a runtime
 *
 * returns : 0 on success, 1 on error
 */
 int write_result(int n, int t, char *prog, double time, graph_t *G);


#endif
