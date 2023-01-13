#!/bin/bash

gcc mm_1.c -DIS_PAPI -std=c99 -lpapi -msse2 2>&1 >/dev/null

#for (( j=0 ; j < 8 ; j++ ))
#for j in 3 7 11 12
#do
#    echo "mode $j -------------------------------"
#    for (( i=1 ; i < 31 ; i++ ))
#    do
#        ./a.out -a $( echo "16*$i" | bc ) -n $( echo "20000/$i^3" | bc ) -m $j
#    done
#done

gcc mm_1.c -DIS_PAPI -std=c99 -lpapi -O3 -msse2 2>&1 >/dev/null

for (( j=11 ; j < 15 ; j++ ))
do
    echo "mode $j -O3 -------------------------------"
    for (( i=1 ; i < 31 ; i++ ))
    do
        ./a.out -a $( echo "16*$i" | bc ) -n $( echo "100000/$i^3" | bc ) -m $j
    done
done
