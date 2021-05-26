#!/bin/bash
#Experiment 1: Idle Lock overhead
M="100"
N="8"
W="500"
U="t"
D="8"
L="p"

num_trials=5


echo "Running correctness experiments..."

#Get a Lock-Free Data point
for (( trial=1; trial<=$num_trials; trial++ )); do
    serial_sum=$(./serial $M $N $W $U "$trial" t)

    H_sum=$(./parallel $M $N $W $U "$trial" $D $L H t)
    A_sum=$(./parallel $M $N $W $U "$trial" $D $L A t)
    L_sum=$(./parallel $M $N $W $U "$trial" $D na L t)
    if [ "$L_sum" = "$serial_sum" ]; then
        true
    else
        echo "ERR: Lock Free: bad output"
    fi

    if [ "$H_sum" = "$serial_sum" ]; then
        true
    else
        echo "ERR: HomeQueue: bad output"
    fi

    if [ "$A_sum" = "$serial_sum" ]; then
        true
    else
        echo "ERR: Awesome: bad output"
    fi
done
