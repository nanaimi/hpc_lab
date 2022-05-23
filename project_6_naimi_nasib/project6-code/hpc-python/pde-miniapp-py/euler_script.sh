#!/usr/bin/env bash

mkdir -p data/

steps=100
t=0.01

echo "--- Scaling ---"
for gridsize in 128 256 512 1024
do
    echo "--- $gridsize dim"
    for p in {1..32}
    do
        echo "--- $p ---"
        bsub -n $p -R "select[model==XeonGold_6150]" -W 01:00 \
            -oo data/$gridsize-$p.txt mpirun python3 main.py $gridsize $steps $t
    done
done

echo "--- jobs submitted ---"
