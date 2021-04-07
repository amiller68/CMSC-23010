/*
When I finally want to test the results of my program, I can write a bash script that does the following:
For a list of values N and list of values T,
    For some n \in N, we require the following results:
        Generate a random graph with n vertices
        Run the serial program on the graph, record the time it takes
        record the result in <n:1>_serial_<time>.txt
        for t \in T:
            run the parallel program on the graph using t threads, record the time it takes
            record the result in  <n:t>_parallel_<time>.txt
This implies that within by bash script, I'm going to want the following structure:
For n \in N
    python graph.py n --> dir/n.txt //an adajacency matrix with n vertices
    ./serial n.txt 1 --> <dir/n:1>_serial_<time>.txt //a result table text file
    for t in T
        ./parallel n.txt t --> <dir/n:t>_parallel_<time>,txt // a result table text file
    Compare the contents of the serial output to the parallel output files using bash
 * Takes as input:
 */
