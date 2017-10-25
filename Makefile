CFLAGS=-g2

.PHONY: clean

default: clean build
	@head -n 2000 velky.txt | ./prd
	@./gen_images.sh

big: build
	./prd < velky.txt

build:
	@gcc $(CFLAGS) prd.c -o prd

p:
	@./gen_images.sh
	@qlmanage -p 2>/dev/null *.png

clean:
	@rm -f -- *.png *.dot
	@rm -f -- prd
