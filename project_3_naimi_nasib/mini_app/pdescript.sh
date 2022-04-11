# for i in {0..4}
# do
  SIZE=1024 # $[64*2**i]
  for t in {1..24}
  do
    export OMP_NUM_THREADS=$t
    bsub -n $t -W 00:10 -R "span[ptile=$t]" -R "select[model=XeonGold_6150]" ./main $SIZE 300 0.05
  done
# done