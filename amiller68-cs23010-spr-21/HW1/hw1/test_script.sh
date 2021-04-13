#!/bin/bash
N="16 32 64 128 256 512 1024"
T="2 4 8 16 32 64"

printf "Testing script...\n"
for n in $N; do
    python3 graph.py $n
    printf 'Created new graph : n = %s\n' "$n"
    file_name="tests/${n}.txt"
    printf 'Running serial program : n = %s\n' "$n"
    ./fw_serial $file_name 1
    s_res_file='find res/ -name "$n:1_*"'
    for t in $T; do
        printf 'Running parallel program : n = %s | t = %s\n' "$n" "$t"
        ./fw_parallel $file_name $t
        p_res_file='find res/ -name "$n:$t_*"'
        if cmp -s "$s_res_file" "$p_res_file"; then
            printf 'SERR: Failed operation <%s:%s>\n' "$n" "$t"
            rm "$p_res_file"
        fi
    done
done
