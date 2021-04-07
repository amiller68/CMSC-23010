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


}
