#include <stdlib.h>
#include <stdio.h>

typedef int num;

#define prlen 0x1000
char pr[prlen];
num pra;
char thisc;

#define dslen 16
num ds[dslen];
num dsp;

#define cslen 16
num cs[cslen];
num csp;

#define stlen 0x10000
#define stpad 16
num st[stlen];
num stp, stlink;

enum { cif = -1, cloop = -2 };

void panic(const char m[])
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

void allot(num n)
{
	if(stp>stlen-stpad) panic("out of memory");
	st[stp++] = n;
}

num define(num name, num value)
{
	allot(name);
	allot(stlink); stlink = stp-2;
	allot(value);
	return stp-1;
}

num find(num name)
{
	num link = stlink;
	while(link)
	{
		if(st[link]==name) break;
		link = st[link+1];
	}
	return link;
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

void dollar(){
	num link = find(drop());
	if(link==0) panic("macro not found");
	cpush(pra);
	pra = st[link+2];
}

void percent(){ num x = drop(); num y = drop(); push(x); push(y); }

void apostrophe(){ while(nextc()!='\n'); }

void leftpar(){ cpush(pra); cpush(cloop); }

void rightpar()
{
	if(csp<2 || cs[csp-1]!=cloop) panic("invalid loop");
	pra = cs[csp-2];
}

void astesrisk(){ push(drop()*drop()); }

void plus(){ push(drop()+drop()); }

void comma(){ allot(drop()); }

void minus(){ num x = drop(); push(drop()-x); }

void dot(){ drop(); }

void slash(){ num x = drop(); push(drop()/x); }

void digit(){ push(drop()*10 + thisc-'0'); }

void colon(){ num x =  drop(); push(x); push(x); }

void semicolon(){ pra = cdrop(); }

void less(){ push(drop()<0); }

void equals(){ st[drop()] = drop(); }

void greater()
{
	num name = drop();
	num link = find(name);
	if(link) push(link+2); else push(define(name, 0));
}

void question(){ num x; scanf("%u", &x); push(x); }

void arrobe()
{
	define(drop(), pra);
	while(nextc()!=';');
}

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

void rightbr(){ if(cdrop()!=cif) panic("control mismatch"); }

void caret()
{
	if(drop()==0)
	{
		if(cdrop()!=cloop) panic("control mismatch");
		skipc('(', ')');
	}
}

void underscore(){ push(st[drop()]); }

void lowercase(){ thisc = thisc + ('A'-'a'); letter(); }

void leftcur(){ push(getc(stdin)); }

void pipe()
{
	if(cdrop()!=cif) panic("control mismatch");
	skipc('[', ']');
}

void rightcur(){ putchar(drop()); }

void (*symbols[256])()={
end, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, space, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,

space, exclamation, quote, hash, dollar, percent, unknown, apostrophe,
leftpar, rightpar, astesrisk, plus, unknown, minus, dot, slash,
digit, digit, digit, digit, digit, digit, digit, digit,
digit, digit, colon, semicolon, less, equals, greater, question,
arrobe, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, letter, letter, letter, letter, letter,
letter, letter, letter, leftbr, unknown, rightbr, caret, underscore,
unknown, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase, lowercase,
lowercase, lowercase, lowercase, leftcur, pipe, rightcur, unknown, unknown,

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
