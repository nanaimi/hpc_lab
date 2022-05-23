#!/usr/bin/env bash

module load gcc/6.3.0 openmpi/3.0.1 python/3.6.4

mkdir -p data_processes/
make clean
make || exit 1

steps=100
t=0.01

echo "--- Scaling ---"
for gridsize in 128 256 512 1024
do
    echo "-- $gridsize dim"
    for nodes in 1
    do
        echo "-- $nodes node/s"
        for p in {1..32}
        do
            # ptile=$(($p / $nodes))
            # if (( $p % $nodes != 0 || $ptile > 24 )); then
            #     continue
            # fi
            echo "- $p/$ptile"
            
            bsub -n $p -R "span[ptile=$nodes]" -R "select[model==XeonGold_6150]" -W 01:00 \
            -oo data_processes/$gridsize-$p-$nodes.txt mpirun ./main $gridsize $steps $t
        done
    done
done

echo "--- jobs submitted ---"