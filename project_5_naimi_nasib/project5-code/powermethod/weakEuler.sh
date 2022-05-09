#!/usr/bin/env bash

module load gcc/6.3.0 openmpi/3.0.1 python/3.6.0 || :

mkdir -p output/weak

make clean
make || exit 1

NUM_ITER=100
N=12288

for nodes in 1 2 3 4
do
    echo "-- $nodes node/s"
    for p in 1 2 4 8 12 16 32 64
    do
        ptile=$(($p / $nodes))
        if (( $p % $nodes != 0 || $ptile > 24 )); then
            continue
        fi

        n=$(python get-n.py $p $N)

        echo "- $p/$ptile n=$n"

        echo "- powermethod"
        bsub -n $p -R "span[ptile=$ptile]" -R "select[model==XeonGold_6150]" -W 01:00 \
        -oo output/weak/$p-$nodes.txt mpirun ./powermethod $n $NUM_ITER

    done
done