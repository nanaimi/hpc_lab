#!/bin/sh
for t in {1..24}
do
export OMP_NUM_THREADS=t
for i in {0..4}
do
  N=100000+10^i
  bsub -n $OMP_NUM_THREADS -W 00:10 -R "span[ptile=$OMP_NUM_THREADS]" ./dotProduct $N
done
done
