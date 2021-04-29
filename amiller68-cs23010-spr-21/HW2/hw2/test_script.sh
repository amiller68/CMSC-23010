#!/bin/bash
#Make a directory to store our experimental data
mkdir -p exp_data

#Initialize directories to hold our immediate tests and results
mkdir -p tmp
mkdir -p res

#Experiment 1: Parallel Overhead
W="200 400 800"
N="2 9 14"
D="32"
opt="u"
num_trials=5

echo "Running exp 1..."
for n in $N; do
    #Header for our exp1_n data
    printf "N, T, W, D, Speedup\n" >> exp_data/exp1_$n.csv
    for w in $W; do
        #Reset our speedup list
        speedup_list=""
        #Generate a T value
        T=$(( (2**20) / ($n * $w) ))
        #Run all trials for a test...
        for (( trial=1; trial<=$num_trials; trial++ )); do
            #echo "$trial"
            ./serial_queue $n $T $w $D "$trial" $opt
            sq_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            sq_time=${sq_res_file##*,}
            mv $sq_res_file tmp/${sq_res_file##*/}

            ./serial $n $T $w $D "$trial" $opt
            s_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            s_time=${s_res_file##*,}

            #If this is a valid result
            if cmp -s $s_res_file tmp/${sq_res_file##*/}; then
                #echo "S_time : $s_time | sq_time : $sq_time\n"

                #calculate speedup for this trial
                speedup=$(echo "$s_time $sq_time" | awk '{printf "%.5f \n", $1/$2}')
                #echo "Speedup : $speedup"

                #Add it to our list of speedups
                speedup_list+=$speedup

                #echo "Speedup list : $speedup_list"
            else
                printf 'EXP1 ERR: Failed operation <%s:%s>\n' "$n" "$w"
                touch exp_data/EXP1_ERR_$n:$w:$trial
            fi
            #Remove extraneous files
            rm tmp/${sq_res_file##*/}
            rm $s_res_file
        done
        #Find the median speedup for this test...
        speedup=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $speedup_list)
        #Split the formatted output ([1] <speedup>)
        speedup=${speedup##* }
        #Write this data into our csv file
        printf "$n, $T, $w, $D, $speedup\n" >> exp_data/exp1_$n.csv
    done
done

#Experiment 2: Dispatcher Rate
W=1
N="2 3 5 9 14 28"
D="32"
opt="u"
num_trials=5

#Header for our exp2 data
printf "N, T, W, D, ratio\n" >> exp_data/exp2.csv

echo "Running exp 2..."
for n in $N; do
    #Reset our speedup list
    ratio_list=""
    #Generate a T value
    T=$(( (2**20) / ($n - 1) ))

    size=$(( $T * ($n - 1) ))

    #echo Size: $size

    #Run all trials for a test...
    for (( trial=1; trial<=$num_trials; trial++ )); do
        #echo Trial: n = $n, trial = $trial

        ./parallel $n $T "$W" $D "$trial" $opt
        p_res_file=$(find res/ -name "$n,$T,"$W",$D,"$trial",$opt,*")
        p_time=${p_res_file##*,}

        #echo Runtime: $p_time

        mv $p_res_file tmp/${p_res_file##*/}

        ./serial $n $T "$W" $D "$trial" $opt
        s_res_file=$(find res/ -name "$n,$T,"$W",$D,"$trial",$opt,*")
        #s_time=${s_res_file##*,}

        #If this is a valid result
        if cmp -s $s_res_file tmp/${p_res_file##*/}; then
            #echo "S_time : $s_time | sq_time : $sq_time\n"

            #calculate speedup for this trial
            ratio=$(echo "$size $p_time" | awk '{printf "%.5f \n", $1/$2}')

            #echo "Ratio : $ratio"

            #Add it to our list of speedups
            ratio_list+=$ratio

            #echo "Speedup list : $speedup_list"
        else
            printf 'EXP2 ERR: Failed operation <%s:%s>\n' "$n" "$W"
            touch exp_data/EXP2_ERR_$n:$W:$trial
        fi
        #Remove extraneous files
        rm tmp/${p_res_file##*/}
        rm $s_res_file
    done
    #echo $ratio_list
    #Find the median speedup for this test...
    ratio=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $ratio_list)
    #Split the formatted output ([1] <speedup>)
    ratio=${ratio##* }
    #Write this data into our csv file
    printf "$n, $T, $W, $D, $ratio\n" >> exp_data/exp2.csv
done

#Experiment 3: Speedup with Constant Load
W="1000 2000 4000 8000"
N="2 3 5 9 14 28"
D="32"
opt="c"
num_trials=5

echo "Running exp 3..."
for w in $W; do
    #Header for our exp1_n data
    printf "N, T, W, D, Speedup\n" >> exp_data/exp3_$w.csv
    for n in $N; do
        #Reset our speedup list
        speedup_list=""
        #Generate a T value
        T=$(( (2**15) ))
        #Run all trials for a test...
        for (( trial=1; trial<=$num_trials; trial++ )); do
            #echo "$trial"
            ./parallel $n $T $w $D "$trial" $opt
            p_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            p_time=${p_res_file##*,}
            mv $p_res_file tmp/${p_res_file##*/}

            ./serial $n $T $w $D "$trial" $opt
            s_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            s_time=${s_res_file##*,}

            #If this is a valid result
            if cmp -s $s_res_file tmp/${p_res_file##*/}; then
                #echo "S_time : $s_time | sq_time : $sq_time\n"

                #calculate speedup for this trial
                speedup=$(echo "$s_time $p_time" | awk '{printf "%.5f \n", $1/$2}')
                #echo "Speedup : $speedup"

                #Add it to our list of speedups
                speedup_list+=$speedup

                #echo "Speedup list : $speedup_list"
            else
                printf 'EXP3 ERR: Failed operation <%s:%s>\n' "$n" "$w"
                touch exp_data/EXP3_ERR_$n:$w:$trial
            fi
            #Remove extraneous files
            rm tmp/${p_res_file##*/}
            rm $s_res_file
        done
        #Find the median speedup for this test...
        speedup=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $speedup_list)
        #Split the formatted output ([1] <speedup>)
        speedup=${speedup##* }
        #Write this data into our csv file
        printf "$n, $T, $w, $D, $speedup\n" >> exp_data/exp3_$w.csv
    done
done

#Experiment 4: Speedup with Uniform Load
W="1000 2000 4000 8000"
N="2 3 5 9 14 28"
D="32"
opt="u"
num_trials=5

echo "Running exp 4..."
for w in $W; do
    #Header for our exp1_n data
    printf "N, T, W, D, Speedup\n" >> exp_data/exp4_$w.csv
    for n in $N; do
        #Reset our speedup list
        speedup_list=""
        #Generate a T value
        T=$(( (2**15) ))
        #Run all trials for a test...
        for (( trial=1; trial<=$num_trials; trial++ )); do
            #echo "$trial"
            ./parallel $n $T $w $D "$trial" $opt
            p_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            p_time=${p_res_file##*,}
            mv $p_res_file tmp/${p_res_file##*/}

            ./serial $n $T $w $D "$trial" $opt
            s_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            s_time=${s_res_file##*,}

            #If this is a valid result
            if cmp -s $s_res_file tmp/${p_res_file##*/}; then
                #echo "S_time : $s_time | sq_time : $sq_time\n"

                #calculate speedup for this trial
                speedup=$(echo "$s_time $p_time" | awk '{printf "%.5f \n", $1/$2}')
                #echo "Speedup : $speedup"

                #Add it to our list of speedups
                speedup_list+=$speedup

                #echo "Speedup list : $speedup_list"
            else
                printf 'EXP4 ERR: Failed operation <%s:%s>\n' "$n" "$w"
                touch exp_data/EXP4_ERR_$n:$w:$trial
            fi
            #Remove extraneous files
            rm tmp/${p_res_file##*/}
            rm $s_res_file
        done
        #Find the median speedup for this test...
        speedup=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $speedup_list)
        #Split the formatted output ([1] <speedup>)
        speedup=${speedup##* }
        #Write this data into our csv file
        printf "$n, $T, $w, $D, $speedup\n" >> exp_data/exp4_$w.csv
    done
done

#Experiment 5: Speedup with Constant Load
W="1000 2000 4000 8000"
N="2 3 5 9 14 28"
D="32"
opt="e"
num_trials=11

echo "Running exp 5..."
for w in $W; do
    #Header for our exp1_n data
    printf "N, T, W, D, Speedup\n" >> exp_data/exp5_$w.csv
    for n in $N; do
        #Reset our speedup list
        speedup_list=""
        #Generate a T value
        T=$(( (2**15) ))
        #Run all trials for a test...
        for (( trial=1; trial<=$num_trials; trial++ )); do
            #echo "$trial"
            ./parallel $n $T $w $D "$trial" $opt
            p_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            p_time=${p_res_file##*,}
            mv $p_res_file tmp/${p_res_file##*/}

            ./serial $n $T $w $D "$trial" $opt
            s_res_file=$(find res/ -name "$n,$T,$w,$D,"$trial",$opt,*")
            s_time=${s_res_file##*,}

            #If this is a valid result
            if cmp -s $s_res_file tmp/${p_res_file##*/}; then
                #echo "S_time : $s_time | sq_time : $sq_time\n"

                #calculate speedup for this trial
                speedup=$(echo "$s_time $p_time" | awk '{printf "%.5f \n", $1/$2}')
                #echo "Speedup : $speedup"

                #Add it to our list of speedups
                speedup_list+=$speedup

                #echo "Speedup list : $speedup_list"
            else
                printf 'EXP3 ERR: Failed operation <%s:%s>\n' "$n" "$w"
                touch exp_data/EXP3_ERR_$n:$w:$trial
            fi
            #Remove extraneous files
            rm tmp/${p_res_file##*/}
            rm $s_res_file
        done
        #Find the median speedup for this test...
        speedup=$(Rscript -e 'median(as.numeric(commandArgs(TRUE)))' $speedup_list)
        #Split the formatted output ([1] <speedup>)
        speedup=${speedup##* }
        #Write this data into our csv file
        printf "$n, $T, $w, $D, $speedup\n" >> exp_data/exp5_$w.csv
    done
done

#Remove our temporary folder
rm -r tmp
