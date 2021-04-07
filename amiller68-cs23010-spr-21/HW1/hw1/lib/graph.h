#ifndef GRAPH_H_
#define GRAPH_H_

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
 * result - a pointer to a 2D array to convert to text
 *
 * file_name - a pointer to a file prefix
 *
 * srt - a double describing a serial runtime
 *
 * prt - a double describing a parallel runtime
 *
 * returns : 0 on success, 1 on error
 */
int write_result(int **result, char *file_name, double srt, double prt);

#endif
