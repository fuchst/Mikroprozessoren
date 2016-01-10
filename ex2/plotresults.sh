#! /bin/bash

for i in `seq 1 100`;
do
    gnuplot -e "filename='${i}.log'; outputname='${i}.png'" gplotscript.txt
done
