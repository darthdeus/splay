#!/bin/bash

for n in 10 100 1000 10000 100000 1000000; do
  ./prd < data-$n.txt > graph-$n.txt
  ./prd --naive < data-$n.txt > graph-naive-$n.txt
done

./prd < data-seq.txt > graph-seq.txt
./prd --naive < data-seq.txt > graph-naive-seq.txt
