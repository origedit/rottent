test: rottent
	./rottent examples/test.rtn

rottent: rottent.c
	cc -std=c90 -Wall -o $@ rottent.c

