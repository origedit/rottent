#include <stdlib.h>
#include <stdio.h>

typedef int num;
/* typedef unsigned int unum;    do i need it? */

/* to do: dynamic program storage */
#define prlen 0x1000
char pr[prlen];
num pra;
char thisc;

#define dslen 32
/* #define dspad */
num ds[dslen];
num dsp;

#define rslen 16
/* #define rspad */
num rs[rslen];
num rsp;

void panic()
{
	exit(1);
}

void push(num n)
{
	if(dsp==dslen){ puts("overflow"); panic(); };
	ds[dsp++] = n;
}

num drop()
{
	if(dsp==0){ puts("underflow"); panic(); };
	return ds[--dsp];
}

char nextc()
{
	char c = pr[pra++];
	if(c==0) panic();
	return c;
}

/* symbols */

void end(){ exit(0); } /* to do : check return stack */

void unknown(){ printf("unknown symbol \"%c\" with value %d and address %d\n", thisc, thisc, pra); panic(); }

void space(){  }

void exclamation(){ printf("%d", drop()); }

void quote(){
	char c;
	while((c = nextc())!='"') putchar(c);
}

void hash(){ push(0); }

void dollar(){ /* to do */ }

void percent(){ /* to do */ }

void apostrophe(){ /* to do */ }

void leftpar(){ /* to do */ }

void rightpar(){ /* to do */ }

void astesrisk(){ push(drop()*drop()); }

void plus(){ push(drop()+drop()); }

void comma(){ /* to do */ }

void minus(){ num x = drop(); push(drop()-x); }

void dot(){ /* to do */ }

void slash(){ num x = drop(); push(drop()/x); }


void digit(){ push(drop()*10 + thisc-'0'); }

void colon(){ /* to do */ }

void semicolon(){ /* to do */ }

void less(){ /* to do */ }

void equals(){ /* to do */ }

void greater(){ /* to do */ }

void question(){ /* to do */ }

void arrobe(){ /* to do */ }

void letter(){ push(drop()*26 + thisc-'A'); }

void leftbr(){ /* to do */ }

void caret(){ /* to do */ }

void rightbr(){ /* to do */ }

void lowercase(){ thisc = thisc + ('A'-'a'); letter(); }

void pipe(){ /* to do */ }

void (*symbols[256])()={
end, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, space, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,

space, exclamation, quote, hash, unknown, unknown, unknown, unknown,
unknown, unknown, astesrisk, plus, unknown, minus, unknown, slash,
digit, digit, digit, digit, digit, digit, digit, digit,
digit, digit, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, unknown, unknown, unknown, unknown, unknown,
unknown, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, unknown, unknown, unknown, unknown, unknown,

unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown
};

int main(int argc, char **argv)
{
	if (argc!=2)
	{
		puts("source file not found");
		return 1;
	}
	FILE *f = fopen(argv[1], "rb");
	pr[fread(pr, 1, prlen, f)] = 0;
	fclose(f);
	/* interpret */
	pra = 0;
	for(;;)
	{
		thisc = pr[pra++];
		symbols[(signed char) thisc]();
	}
	
	return 0;
}
