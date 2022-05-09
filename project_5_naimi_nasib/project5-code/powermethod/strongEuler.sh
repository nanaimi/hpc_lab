#!/usr/bin/env bash

module load gcc/6.3.0 openmpi/3.0.1

mkdir -p output/strong
make clean
make || exit 1

NUM_ITER=1000
N=12288

echo "--- Strong Scaling ---"
for nodes in 1 2 3 4
do
    echo "-- $nodes node/s"
    for p in 1 2 4 6 8 12 16 24 32 48 64
    do
        ptile=$(($p / $nodes))
        if (( $p % $nodes != 0 || $ptile > 24 )); then
            continue
        fi
        echo "- $p/$ptile"

        bsub -n $p -R "span[ptile=$ptile]" -R "select[model==XeonGold_6150]" -W 01:00 \
        -oo output/strong/g$p-$nodes.txt mpirun ./powermethodg $N $NUM_ITER
        
    done
done