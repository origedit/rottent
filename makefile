test: rottent
	./rottent examples/test.rtn

clean:
	rm rottent

rottent: rottent.c
	cc -std=c90 -Wall -o $@ rottent.c

install: rottent
	# don't run this unless you have bin/ in ~/
	cp rottent ~/bin/
