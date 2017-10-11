default:
	@gcc prd.c -o prd
	@head -n 2000 maly.txt | ./prd
