
for t in {1..24}
do
    export OMP_NUM_THREADS=$t
    bsub -n $t -W 00:10 -R "span[ptile=$t]" ./mandel_omp
done
