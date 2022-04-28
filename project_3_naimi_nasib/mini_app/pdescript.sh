# for i in {0..4}
# do
  SIZE=64 # $[64*2**i]
  for t in {21..24}
  do
    export OMP_NUM_THREADS=$t
    bsub -n $t -W 00:05 -R "span[ptile=$t]" -R "select[model=XeonGold_6150]" ./main $SIZE 100 0.005
  done
# done
