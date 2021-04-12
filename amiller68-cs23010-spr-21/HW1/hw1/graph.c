#include "lib/graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* see lib/graph.h */
graph_t *construct_graph(char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "r");

    if(!fp)
    {
        printf("No such file: %s", file_name);
        return NULL;
    }

    char *dir = strtok(file_name, "/");
    char *num_vch = strtok(NULL, ".");
    int num_v = atoi(num_vch);

    graph_t *G = (graph_t *) malloc(sizeof(graph_t));
    G->num_v = num_v;
    G->b_i = 0; //A block's i dimension
    G->b_j = 0; //A block's j dimension
    G->k = 0; //What value of k has been calcualated?

    //printf("This graph should have %d vertices\n", num_v);

    int tmp, scan;
    for (int i = 0; i < num_v; i++)
    {
        for (int j = 0; j < num_v; j++)
        {
            scan = fscanf(fp, "%d", &tmp);
            if (scan < 0)
            {
                printf("Reached EOF!");
            }
            G->M[i][j] = MIN(tmp, MAX_EDGE + 1);
        }
    }

    fclose(fp);
    return G;
}

/* see lib/graph.h */
void print_graph(graph_t *G)
{
    int num_v = G->num_v;
    for(int i = 0; i < num_v; i++)
    {
        for(int j = 0; j < num_v; j++)
        {
            printf("%d ", G->M[i][j]);
        }
        printf("\n");
    }

    return;
}

void print_sq(graph_t *G, int i, int j)
{
    int num_v = G->num_v;

    for(int x = i; x < i + G->b_i; x++)
    {
        for(int y = j; y < j + G->b_j; y++)
        {
            printf("%d ", G->M[x][y]);
        }
        printf("\n");
    }

    return;
}


/* see lib/graph.h */
int write_result(int n, int t, char *prog, double time, graph_t *G)
{
    /*
     file_name format:
        <n>:<t>_<prog>_<time>.txt
    */

    char file_name[100];
    char int_buff[10];
    int num_v = G->num_v;
    FILE *fp;

    sprintf(file_name, "res/%d:%d_%s_%f.txt", n, t, (prog + 2), time);

    fp = fopen(file_name, "w+");

    if(!fp)
    {
        printf("No such file: %s", file_name);
        return 1;
    }

    for(int i = 0; i < num_v; i++)
    {
        for(int j = 0; j < num_v; j++)
        {
            sprintf(int_buff, "%d ", G->M[i][j]);
            fputs(int_buff, fp);
        }
        fputs("\n", fp);
    }

    return 0;
}
