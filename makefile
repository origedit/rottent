test: rottent
	./rottent examples/test.rtn

clean:
	rm rottent

rottent: rottent.c
	cc -std=c90 -Wall -o $@ rottent.c

noodle:
	noodle logo-10x10.icn

