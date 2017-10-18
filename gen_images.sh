#!/bin/sh
for f in *.dot; do
  dot -Tpng $f > ${f%.dot}.png
done
