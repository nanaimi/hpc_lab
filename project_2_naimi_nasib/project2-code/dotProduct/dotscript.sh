for t in {1..24}
do
export OMP_NUM_THREADS=t
for i in {0..4}
do
  N=100000+10^i
  bsub -n t -W 00:10 -R "span[ptile=t]" ./dotProduct $N
done
done