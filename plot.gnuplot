set term png

set output "plot-1.png"

plot "graph-10.txt" with lines, \
     "graph-100.txt" with lines, \
     "graph-1000.txt" with lines, \
     "graph-10000.txt" with lines, \
     "graph-100000.txt" with lines, \
     "graph-1000000.txt" with lines

set output "plot-2.png"

plot "graph-naive-10.txt" with lines, \
     "graph-naive-100.txt" with lines, \
     "graph-naive-1000.txt" with lines, \
     "graph-naive-10000.txt" with lines, \
     "graph-naive-100000.txt" with lines, \
     "graph-naive-1000000.txt" with lines

set output "plot-3.png"

plot "graph-100.txt" with lines, \
     "graph-10000.txt" with lines, \
     "graph-1000000.txt" with lines, \
     "graph-naive-100.txt" with lines, \
     "graph-naive-10000.txt" with lines, \
     "graph-naive-1000000.txt" with lines

set output "plot-4.png"

set logscale y

plot "graph-seq.txt" with lines, \
     "graph-naive-seq.txt" with lines

