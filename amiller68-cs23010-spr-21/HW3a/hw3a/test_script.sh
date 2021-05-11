#!/bin/bash
#Make a directory to store our experimental data
mkdir -p exp_data

#Experiment 1: Idle Lock overhead
B="10000"
N="1"
opt="t p a m"
num_trials=5

echo "Running exp 1..."

#Header for our exp
printf "B,n,lock,Serial Throughput,Speedup\n" >> exp_data/exp1.csv

rate_list=""

#Get data from serial counter

echo Getting serial throughput...

for (( trial=1; trial<=$num_trials; trial++ )); do
    res=$(./serial $B)
    s_count=${res%%,*}
    s_time=${res##*,}

    if [ "$s_count" = "$B" ]; then
        rate=$(echo "$B $s_time" | awk '{printf "%.5f \n", $1/$2}')
        rate_list+=$rate
    else
        echo "ERR-1: serial counter: bad output"
        touch exp_data/EXP1_ERR_SERIAL:$B:$trial
    fi
done

#Find the median worker rate for this test...
rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
#Split the formatted output ([1] <rate>)
echo Using serial throughput = ${rate##* }
serial_rate=${rate##* }

printf "$B,$N,s,$serial_rate,1.0\n" >> exp_data/exp1.csv

for l in $opt; do
    echo Testing lock type: $l
    #Reset the rate list
    rate_list=""
    for (( trial=1; trial<=$num_trials; trial++ )); do
        res=$(./parallel $B 1 $l)
        p_count=${res%%,*}
        p_time=${res##*,}

        if [ "$p_count" = "$B" ]; then
            rate=$(echo "$B $p_time" | awk '{printf "%.5f \n", $1/$2}')
            rate_list+=$rate
        else
            echo "ERR-1: parallel counter: bad output"
            touch exp_data/EXP1_ERR_PARALLEL:$B:$l:$trial
        fi
    done
    #Find the median worker rate for this test...
    rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
    #Split the formatted output ([1] <rate>)
    echo Using parrallel throughput = ${rate##* }
    parallel_rate=${rate##* }

    #Get the speedup by dividing throughput rates
    speedup=$(echo "$serial_rate $parallel_rate" | awk '{printf "%.5f \n", $1/$2}')
    echo Effectvie speedup = $speedup X

    printf "$B,$N,$l,$serial_rate,$speedup\n" >> exp_data/exp1.csv
done

#Experiment 2: Lock Scaling
B="10000"
N="1 2 4 8 14"
opt="t p a m"
num_trials=5

echo "Running exp 2..."

for l in $opt; do
    #Header for our exp
    echo Testing lock type: $l
    printf "B,n,lock,Serial Throughput,Speedup\n" >> exp_data/exp2_$l.csv
    for n in $N; do
        #Reset the rate list
        echo Running count parallel, n = $n, base rate = $serial_rate
        rate_list=""
        for (( trial=1; trial<=$num_trials; trial++ )); do
            res=$(./parallel $B $n $l)
            p_count=${res%%,*}
            p_time=${res##*,}

            if [ "$p_count" = "$B" ]; then
                rate=$(echo "$B $p_time" | awk '{printf "%.5f \n", $1/$2}')
                rate_list+=$rate
            else
                echo "ERR-2: parallel counter: bad output"
                touch exp_data/EXP2_ERR_PARALLEL:$B:$n:$l:$trial
            fi
        done
        #Find the median worker rate for this test...
        rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
        #Split the formatted output ([1] <rate>)
        echo Using parallel rate = ${rate##* }
        parallel_rate=${rate##* }

        #Get the speedup by dividing throughput rates
        speedup=$(echo "$serial_rate $parallel_rate" | awk '{printf "%.5f \n", $1/$2}')

        printf "$B,$n,$l,$serial_rate,$speedup\n" >> exp_data/exp2_$l.csv
    done
done

#Experiment 3: Critical Sleeping
B="3136"
N="1 2 4 8 14"
T="1 2 4 8 14"
opt="t p a m"
num_trials=5

echo "Running exp 3..."

rate_list=""
s_time_list=""

#Get data from serial counter
for t in $T; do
    echo Running serial sleep...t = $t
    for (( trial=1; trial<=$num_trials; trial++ )); do
        res=$(./my_test_serial $B $t)
        rate_list+=$res
    done
    #Find the median worker rate for this test...
    rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
    echo Runtime = ${rate##* }
    #Split the formatted output ([1] <rate>)
    s_time_list+="${rate##* } "
done

s_time_array=($s_time_list)

for l in $opt; do
    echo Testing lock type: $l
    for n in $N; do
        i=0
        printf "B,t,n,lock,Speedup\n" >> exp_data/exp3_$l:$n.csv
        for t in $T; do
            s_rate=${s_time_array[$i]}
            echo Running sleep parallel sleep, n = $n, t = $t, base rate = $s_rate
            i=$((i + 1))
            #Reset the rate list
            rate_list=""
            for (( trial=1; trial<=$num_trials; trial++ )); do
                res=$(./my_test_parallel $B $t $n $l)
                rate=$res
                rate_list+=$rate
                #echo "ERR: serial counter: bad output"
                #touch exp_data/EXP3_ERR_PARALLEL:$B:$t:$n:$l:$trial
            done
            #echo $rate_list
            #Find the median worker rate for this test...
            rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
            #Split the formatted output ([1] <rate>)
            echo Runtime = ${rate##* }
            p_rate=${rate##* }

            #Get the speedup by dividing throughput rates
            speedup=$(echo "$p_rate $s_rate" | awk '{printf "%.5f \n", $1/$2}')

            printf "$B,$t,$n,$l,$speedup\n" >> exp_data/exp3_$l:$n.csv
        done
    done
done
