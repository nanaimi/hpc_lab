#!/usr/bin/env bash

module load gcc/6.3.0 openmpi/3.0.1

mkdir -p output/strong
make clean
make || exit 1

NUM_ITER=1000
N=12288

echo "--- Strong Scaling ---"

for p in 1 2 4 6 8 12 16 24 32 48
do
    bsub -n $p -R "select[model==XeonGold_6150]" -W 01:00 \
    -oo output/strong/g$p-$nodes.txt mpirun ./powermethodg $N $NUM_ITER
    
done
