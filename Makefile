CFLAGS=-g2

default:
	@gcc $(CFLAGS) prd.c -o prd
	@head -n 2000 maly.txt | ./prd
