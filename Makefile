CFLAGS=-g2 -O2 -march=native

.PHONY: clean build

default: clean build
	@head -n 2000 velky.txt | ./prd
	@./gen_images.sh

big: build
	./prd < data-10.txt

build:
	gcc $(CFLAGS) prd.c -o prd

p:
	@./gen_images.sh
	@qlmanage -p 2>/dev/null *.png

clean:
	@rm -f -- *.png *.dot
	@rm -f -- prd

run_all:
	./run_all.sh

plot:
	gnuplot plot.gnuplot
