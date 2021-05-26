#!/bin/bash
mkdir -p exp_data

exp1=""
exp2=""

exp1="t"
exp2="t"



#Experiment 1: Idle Lock overhead
M="200"
N="1"
W="25 50 100 200 400 800"
U="t"
D="8"
L="p a"

num_trials=1

if [[ $exp1 = "t" ]]
then
    printf "M,n,W,U,D,L,Lock-Free Rate,Home-Queue Rate,Speedup\n" >> exp_data/overhead.csv

    echo "Running overhead experiments..."

    for w in $W; do
        rate_list=""
        #Get a Lock-Free Data point
        for (( trial=1; trial<=$num_trials; trial++ )); do
            throughput=$(./parallel $M $N $w $U "$trial" $D nan L)


            rate=$(echo "$throughput $M" | awk '{printf "%.5f \n", $1/$2}')
            rate_list+=$rate
        done

        #Find the median worker rate for this test...
        rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
        #Split the formatted output ([1] <rate>)
        echo Using Lock-Free Rate = ${rate##* }

        #Out LF data point
        LF_rate=${rate##* }


        for l in $L; do
            rate_list=""
            #Get a Home-QUeue Data point
            for (( trial=1; trial<=$num_trials; trial++ )); do
                throughput=$(./parallel $M $N $w $U "$trial" $D $l H)

                rate=$(echo "$throughput $M" | awk '{printf "%.5f \n", $1/$2}')
                rate_list+=$rate
            done

            #Find the median worker rate for this test...
            rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
            #Split the formatted output ([1] <rate>)
            echo Using Home-Queue Rate = ${rate##* }

            #Our HQ data point
            HQ_rate=${rate##* }

            #The faster LF is than HQ, the larger this value is. Perhpas this better termed slowdown
            speedup=$(echo "$LF_rate $HQ_rate" | awk '{printf "%.5f \n", $1/$2}')

            printf "$M,$N,$w,$U,$D,$l,$LF_rate,$HQ_rate,$speedup\n" >> exp_data/overhead.csv
        done
    done
fi

#Speedup with Varying Loads, straetgies, and locks
M="200"
N="1 2 3 7 13 27"
W="1000 2000 4000 8000"
U="t f"
D="8"
L="p a"
S="H A"

num_trials=1

if [[ $exp2 = "t" ]]
then
    echo "Running speedup experiments..."

    printf "M,n,W,U,D,L,S,Speedup\n" >> exp_data/speedup.csv

    for u in $U; do

        #If this is an test utilizing Uniform packets
        if [[ $u = "t" ]]
        then
            num_trials=1
        else
            num_trials=1
        fi

        for w in $W; do
            for n in $N; do
                #Get a serial rate
                rate_list=""
                for (( trial=1; trial<=$num_trials; trial++ )); do
                    throughput=$(./serial $M $n $w $u "$trial")
                    #echo $throughput
                    rate=$(echo "$throughput $M" | awk '{printf "%.5f \n", $1/$2}')
                    rate_list+=$rate
                done
                #Find the median worker rate for this test...
                rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
                #Split the formatted output ([1] <rate>)
                echo Using Serial Rate = ${rate##* } packet / ms
                s_rate=${rate##* }

                rate_list=""
                #Get a Lock-Free Data point
                for (( trial=1; trial<=$num_trials; trial++ )); do
                    throughput=$(./parallel $M $n $w $u "$trial" $D nan L)

                    rate=$(echo "$throughput $M" | awk '{printf "%.5f \n", $1/$2}')
                    rate_list+=$rate
                done

                #Find the median worker rate for this test...
                rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
                #Split the formatted output ([1] <rate>)
                echo Using Lock-Free Rate = ${rate##* } packet / ms
                LF_rate=${rate##* }

                speedup=$(echo "$LF_rate $s_rate" | awk '{printf "%.5f \n", $1/$2}')

                printf "$M,$n,$w,$u,$D,nan,L,$speedup\n" >> exp_data/speedup.csv

                for s in $S; do
                    if [[ $s = "H" ]]
                    then
                        strat="Home-Queue"
                    else
                        strat="Awesome"
                    fi
                    for l in $L; do
                        rate_list=""
                        #Get a Home-QUeue Data point
                        for (( trial=1; trial<=$num_trials; trial++ )); do
                            throughput=$(./parallel $M $n $w $u "$trial" $D $l $s)

                            rate=$(echo "$throughput $M" | awk '{printf "%.5f \n", $1/$2}')
                            rate_list+=$rate
                        done

                        #Find the median worker rate for this test...
                        rate=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $rate_list)
                        #Split the formatted output ([1] <rate>)
                        echo Using $strat rate = ${rate##* } packet / ms

                        #Our HW data point
                        strat_rate=${rate##* }

                        #The faster LF is than HQ, the larger this value is. Perhpas this better termed slowdown
                        speedup=$(echo "$strat_rate $s_rate" | awk '{printf "%.5f \n", $1/$2}')

                        printf "$M,$n,$w,$u,$D,$l,$s,$speedup\n" >> exp_data/speedup.csv
                    done
                done
            done
        done
    done
fi
