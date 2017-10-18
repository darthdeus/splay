CFLAGS=-g2

.PHONY: clean

default: clean
	@gcc $(CFLAGS) prd.c -o prd
	@head -n 2000 maly.txt | ./prd
	@./gen_images.sh

p:
	@./gen_images.sh
	@qlmanage -p 2>/dev/null *.png

clean:
	@rm -f -- *.png *.dot
	@rm -f -- prd
