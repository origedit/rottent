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

#define cslen 16
/* #define cspad */
num cs[cslen];
num csp;

#define stlen 0x10000
num st[stlen];
num stp, stlink;

enum { cif = -1, cloop = -2 };

void panic(const char m[]) /* to do: deallocation */
{
	fflush(stdout);
	fprintf(stderr, "%s at character no. %d \"%c\" (ascii %d)\n", m, pra, thisc, thisc);
	exit(1);
}

void push(num n)
{
	if(dsp==dslen) panic("overflow");
	ds[dsp++] = n;
}

num drop()
{
	if(dsp==0) panic("underflow");
	return ds[--dsp];
}

void cpush(num n)
{
	if(csp==cslen) panic("control overflow");
	cs[csp++] = n;
}

num cfetch() { return cs[csp-1]; }

num cdrop()
{
	if(csp==0) panic("control underflow");
	return cs[--csp];
}

char nextc()
{
	char c = pr[pra++];
	if(c==0) panic("unexpected end of program");
	return c;
}

void skipc(char left, char right)
{
	int level = 1;
	char c;
	do
	{
		c = nextc();
		if(c==left) ++level; else if(c==right) --level;
	}
	while(level);
}

/* symbols */

void end(){
	if(csp!=0) panic("control mismatch");
	exit(0);
}

void unknown(){ panic("unknown symbol"); }

void space(){  }

void exclamation(){ printf("%d", drop()); }

void quote(){
	char c;
	while((c = nextc())!='"') putchar(c);
}

void hash(){ push(0); }

void dollar(){ /* to do */ }

void percent(){ num x = drop(); num y = drop(); push(x); push(y); }

void apostrophe(){ while(nextc()!='\n'); }

void leftpar(){ cpush(cloop); }

void rightpar()
{
	int level = 0;
	char c;
	do
	{
		if(pra==0) panic("control mismatch");
		c = pr[--pra];
		if(c==')') ++level; else if(c=='(') --level;
	}
	while(level);
	++pra;
}

void astesrisk(){ push(drop()*drop()); }

void plus(){ push(drop()+drop()); }

void comma(){ /* to do */ }

void minus(){ num x = drop(); push(drop()-x); }

void dot(){ drop(); }

void slash(){ num x = drop(); push(drop()/x); }

void digit(){ push(drop()*10 + thisc-'0'); }

void colon(){ num x =  drop(); push(x); push(x); }

void semicolon(){ /* to do */ }

void less(){ push(drop()<0 ? -1 : 0); }

void equals(){ /* to do */ }

void greater(){ /* to do */ }

void question(){ num x; scanf("%u", &x); push(x); }

void arrobe(){ /* to do */ }

void letter(){ push(drop()*26 + thisc-'A'); }

void leftbr()
{
	if(drop())
	{
		cpush(cif);
	}
	else
	{
		int level = 1;
		char c;
		do
		{
			c=nextc();
			if(level==1 && c=='|')
			{
				cpush(cif);
				break;
			}
			if(c=='[') ++level; else if(c==']') --level;
		}
		while(level);
	}
}

void caret()
{
	if(drop()==0)
	{
		if(cdrop()!=cloop) panic("control mismatch");
		skipc('(', ')');
	}
}

void rightbr(){ if(cdrop()!=cif) panic("control mismatch"); }

void lowercase(){ thisc = thisc + ('A'-'a'); letter(); }

void pipe()
{
	if(cdrop()!=cif) panic("control mismatch");
	while(nextc()!=']');
}

void (*symbols[256])()={
end, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, space, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,

space, exclamation, quote, hash, unknown, percent, unknown, apostrophe,
leftpar, rightpar, astesrisk, plus, unknown, minus, dot, slash,
digit, digit, digit, digit, digit, digit, digit, digit,
digit, digit, colon, unknown, less, unknown, unknown, question,
unknown, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, leftbr, unknown, rightbr, caret, unknown,
unknown, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, unknown, pipe, unknown, unknown, unknown,

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
		fputs("provide the source file\n", stderr);
		return 1;
	}
	FILE *f = fopen(argv[1], "rb");
	if(f==NULL)
	{
		fputs("source file not found\n", stderr);
		return 1;
	}
	pr[fread(pr, 1, prlen, f)] = 0;
	fclose(f);
	/* interpret */
	stlink = 0, stp = 1;
	pra = 0;
	for(;;)
	{
		thisc = pr[pra++];
		symbols[(signed char) thisc]();
	}
	return 0;
}
