test: rottent test.rtn
	./rottent test.rtn

rottent: rottent.c
	cc -std=c90 -Wall -o $@ rottent.c

