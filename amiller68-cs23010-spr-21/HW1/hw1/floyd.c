#include "lib/floyd.h"

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
                    G->M[i][j] = G->M[i][k] + G->M[k][j];
                }
            }
        }
    }

    return 0;
}

int fw_parallel(graph_t *G, int p)
{
    return 0;
}
