#! /bin/bash

mkdir -p logs
for i in `seq 1 100`;
do
    ./ex2 | tee logs/${i}.log
done
