#!/bin/sh

for f in $(ls 2>/dev/null *.dot); do
  dot -Tpng $f > ${f%.dot}.png
done
