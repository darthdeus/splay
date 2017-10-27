#!/bin/bash

gcc -O2 -march=native splaygen.c -o splaygen

for n in 10 100 1000 10000 100000 1000000; do
  ./splaygen -s 23 -t $n > data-$n.txt
done

./splaygen -b -s 23 > data-seq.txt
