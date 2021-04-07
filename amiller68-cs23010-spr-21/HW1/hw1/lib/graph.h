#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdbool.h>

typedef struct graph {
    int V;
    int **M;
} graph_t;


/*
 * construct_graph : constructs a graoh from an appropriately formatted text file
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

/*
 * compare_graphs : compare the contents of two graphs
 *
 * a - a pointer to a graph_t instance
 *
 * b - a pointer to a graph_t instance
 *
 * returns : True if the graphs match, False if otherwise
 */
bool compare_graphs(graph_t *a, graph_t *b);


#endif
