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

#gcc mm_1.c -DIS_PAPI -std=c99 -lpapi -msse2

#for j in 0 1 2 3
#do
#    echo "mode $j -------------------------------"
#    for (( i=1 ; i < 31 ; i++ ))
#    do
#        ./a.out -a $( echo "16*$i" | bc ) -n $( echo "10000/$i^3+1" | bc ) -m $j
#    done
#done

#~ gcc mm_1.c -DIS_PAPI -std=c99 -lpapi -msse2
#~ 
#~ for j in 11
#~ do
    #~ echo "mode $j -------------------------------"
    #~ for (( i=1 ; i < 31 ; i++ ))
    #~ do
        #~ ./a.out -a $( echo "16*$i" | bc ) -n $( echo "50000/$i^3+1" | bc ) -m $j
    #~ done
#~ done

#~ gcc mm_1.c -DIS_PAPI -std=c99 -lpapi -msse2 -DCLS=$( getconf LEVEL1_DCACHE_LINESIZE )
#~ 
#~ for j in 17
#~ do
    #~ echo "mode $j -------------------------------"
    #~ for (( i=1 ; i < 31 ; i++ ))
    #~ do
        #~ ./a.out -a $( echo "16*$i" | bc ) -n $( echo "30000/$i^3+1" | bc ) -m $j
    #~ done
#~ done

gcc mm_1.c -DIS_PAPI -O3 -std=c99 -lpapi -msse2

k=16

for j in 17
do
    echo "mode -O3 $j -------------------------------"
    for ( $i=16 ; $i<5000; $i+=$k )
    do
        ./a.out -a $i -n $( echo "200000/(($i/16)^3)+1" | bc ) -m $j
    done
    
    if [ $i -gt 1023 ]; then
        k=32
    fi
done

gcc mm_1.c -DIS_PAPI -O3 -std=c99 -lpapi -msse2 -DCHUNK=16

for j in 10 11 17
do
    echo "mode -O3 $j -------------------------------"
    for ( $i=16 ; $i<5000; $i+=$k )
    do
        ./a.out -a $i -n $( echo "200000/(($i/16)^3)+1" | bc ) -m $j
    done
    
    if [ $i -gt 1023 ]; then
        k=32
    fi
done
