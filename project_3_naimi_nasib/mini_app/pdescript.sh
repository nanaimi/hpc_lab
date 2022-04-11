for i in {0..4}
do
  SIZE=$[64*2**i]
  for t in {1..24}
  do
    export OMP_NUM_THREADS=$t
    bsub -n $t -W 00:10 -R "span[ptile=$t]" ./main $SIZE 100 0.005
  done
done