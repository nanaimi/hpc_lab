for i in {0..3}
do
  N=$[100000*10**i]
  echo $N
  for t in {1..24}
  do
    export OMP_NUM_THREADS=$t
    bsub -n $t -W 00:10 -R "span[ptile=$t]" -R "select[model=XeonGold_6150]" ./dotProduct $N
  done
done
