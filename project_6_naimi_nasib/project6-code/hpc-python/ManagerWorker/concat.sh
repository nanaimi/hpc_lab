#!/bin/bash

count=0
write_next=0
for filename in data/*.txt; do
    if [ "$count" == "0" ]; then
        echo "processes, runtime" >> ms_log.csv
        count=$((count+1))
    fi
    
    while read line; do
        if [ "$write_next" == "1" ] ; then
            echo $line >> ms_log.csv
            let write_next=0
            break
        fi
        if [[ "$line" == "### END TIME ###" ]] ; then
            let write_next=1
        fi
    done < $filename

done
