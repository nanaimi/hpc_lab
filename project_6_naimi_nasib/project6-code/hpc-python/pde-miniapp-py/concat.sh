#!/bin/bash

count=0
write_next=0
for filename in data/*.txt; do
    if [ "$count" == "0" ]; then
        echo "processes, gridsize, time, conjGradientIters, iters/sec, newtonIters\n" >> log.csv
        count=$((count+1))
    fi
    
    while read line; do
        if [ "$write_next" == "1" ] ; then
            echo $line >> log.csv
            let write_next=0
            break
        fi
        if [[ "$line" == "processes, gridsize, time, conjGradientIters, iters/sec, newtonIters" ]] ; then
            let write_next=1
        fi
    done < $filename

done
