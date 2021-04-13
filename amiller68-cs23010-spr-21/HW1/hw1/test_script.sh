#!/bin/sh
N="16 32 64 128 256 512 1024"
T="2 4 8 16 32 64"

#Make a directory to store our experimental data
mkdir -p exp_data

#Header for our exp1 data
printf "Number of vertices, Number of threads, Run-time, Serial Time\n" >> exp_data/exp1.csv

#Create a directory to store our experimental results (exp1)
mkdir -p exp1
for n in $N; do
    #Make a new graph with n vertices
    python3 graph.py $n
    file_name="tests/${n}.txt"

    #Generate a data point for experiement1
    ./fw_parallel $file_name 1

    #Get the path of this file; this is a unique pattern at this point
    p_res_file=$(find res/ -name "$n,1,*")

    #Move the result to the exp1 folder
    mv $p_res_file exp1/${p_res_file##*/}

    #Generate the serial result to judge overhead/speedup
    ./fw_serial $file_name 1

    #Get the file name, including the path; this is a uniqie pattern at this point
    s_res_file=$(find res/ -name "$n,1,*")

    #Extract the runtime from the serial result
    s_time=${s_res_file##*,}

    #If this is a valid result
    if cmp -s $s_res_file exp1/${p_res_file##*/}; then
        #Rename the exp1 data point for ease of analysis
        mv exp1/${p_res_file##*/} exp1/${p_res_file##*/},$s_time
    else
        printf 'SERR: Failed operation <%s:%s>\n' "$n" "$t"
        rm exp1/${p_res_file##*/}
    fi

    #Create a directory to store experimental results (exp2, n = $n)
    mkdir -p exp2/$n

    #Header for our exp2, n = $n data
    printf "Number of vertices, Number of threads, Run-time, Serial Time\n" >> exp_data/exp2_$n.csv
    for t in $T; do
        #printf 'Running parallel program : n = %s | t = %s\n' "$n" "$t"
        ./fw_parallel $file_name $t
        p_res_file=$(find res/ -name "$n,$t,*")
        if cmp -s $s_res_file $p_res_file; then
            #Rename the exp1 data point for ease of analysis
            mv $p_res_file exp2/$n/${p_res_file##*/},$s_time
        else
            printf 'SERR: Failed operation <%s:%s>\n' "$n" "$t"
            rm $p_res_file
        fi
    done
    ls exp2/$n >> exp_data/exp2_$n.csv
done

ls exp1 >> exp_data/exp1.csv
