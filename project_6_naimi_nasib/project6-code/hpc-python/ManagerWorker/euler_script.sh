#!/usr/bin/env bash

mkdir -p data/

steps=100
t=0.01

echo "--- Scaling ---"

for p in 2 4 8 16
do
    for workload in 50 100
    do
        echo "--- $p ---"
        bsub -n $p -R "select[model==XeonGold_5118]" -W 01:00 \
          -oo data/$p_$workload.txt mpirun python3 manager_worker.py 4001 4001 $workload
    done
done

echo "--- jobs submitted ---"
