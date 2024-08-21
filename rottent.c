#include <stdlib.h>
#include <stdio.h>

typedef int num;

#define prlen 0x1000
char pr[prlen];
num pra;
char thisc;

#define dslen 32
/* it really should be 10 */
num ds[dslen];
num dsp;

#define cslen 32
struct csx{
	enum{ cif = -1, cloop = -2, cmacro = -3 }type;
	num pos;
};
struct csx cs[cslen];
num csp;

#define stlen 0x10000
#define stpad 16
num st[stlen];
num stp, stlink;

void panic(const char m[])
{
	fflush(stdout);
	fprintf(stderr, "%s at character no. %d \"%c\" (ascii %d)\n", m, pra, thisc, thisc);
	exit(1);
}

void push(num n)
{
	if(dsp==dslen) panic("too many numbers");
	ds[dsp++] = n;
}

num drop()
{
	if(dsp==0) panic("no numbers left");
	return ds[--dsp];
}

void cpush(int type, num n)
{
	if(csp==cslen) panic("decisions too deep");
	cs[csp++] = (struct csx){type, n};
}

struct csx cfetch()
{
	if(csp==0) panic("broken control");
	return cs[csp-1];
}

struct csx cdrop()
{
	if(csp==0) panic("broken control");
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
	while(link && st[link]!=name) link = st[link+1];
	return link;
}

char nextc()
{
	char c = pr[pra++];
	if(c==0) panic("unexpected end of the program");
	return c;
}

void skipc(char left, char right)
{
	int level = 1;
	char c;
	do
		if((c = nextc())==left) ++level; else if(c==right) --level;
	while(level);
}

int main(int argc, char *argv[])
{
	if (argc<2)
	{
		fputs("provide the program file\n", stderr);
		return 1;
	}
	FILE *f = fopen(argv[1], "rb");
	if(f==NULL)
	{
		fprintf(stderr, "can't open the program file \"%s\"\n", argv[1]);
		return 1;
	}
	pr[fread(pr, 1, prlen, f)] = 0;
	fclose(f);
	
	/* interpret */
	stlink = 0, stp = 1;
	pra = 0;
	for(;;) switch(thisc = pr[pra++]){
	case '\0':
		if(csp!=0) panic("unexpected end of program");
		exit(0);
		break;
	case 1: case 2: case 3: case 4: case 5: case 6: case 7:
	case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: 
	case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
	case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
	case 32: break;
	case '!': printf("%d", drop()); break;
	case '"':{
		char c;
		while((c = nextc())!='"') putchar(c);
	}break;
	case '#': push(0); break;
	case '$':{
		num link = find(drop());
		if(link==0) panic("macro not found");
		cpush(cmacro, pra);
		pra = st[link+2];
	}break;
	case '%':{ num x = drop(), y = drop(); push(x); push(y); }break;
	case '\'': while(nextc()!='\n'); break;
	case '(': cpush(cloop, pra); break;
	case ')':
		if(cfetch().type!=cloop) panic("bad loop");
		pra = cfetch().pos;
		break;
	case '*': push(drop()*drop()); break;
	case '+': push(drop()+drop()); break;
	case ',': allot(drop()); break;
	case '-':{ num x = drop(); push(drop()-x); }break;
	case '.': drop(); break;
	case '/':{
		num x = drop();
		if(x==0) panic("division by zero");
		push(drop()/x);
	}break;
	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		push(drop()*10 + thisc-'0'); break;
	case ':':{ num x =  drop(); push(x); push(x); }break;
	case ';':{
		struct csx c = cdrop();
		if(c.type!=cmacro) panic("bad macro");
		pra = c.pos;
	}break;
	case '<': push(drop()<0); break;
	case '=': st[drop()] = drop(); break;
	case '>':{
		num name = drop();
		num link = find(name);
		if(link) push(link+2); else push(define(name, 0));
	}break;
	case '?':{ num x; scanf("%u", &x); push(x); }break;
	case '@': define(drop(), pra); while(nextc()!=';'); break;
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
	case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
	case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
	case 'Y': case 'Z':
		push(drop()*26 + thisc-'A'); break;
	case '[':
		if(drop()) cpush(cif, 0);else
		{
			int level = 1;
			char c;
			do
			{
				c=nextc();
				if(level==1 && c=='|')
				{
					cpush(cif, 0);
					break;
				}
				if(c=='[') ++level; else if(c==']') --level;
			}
			while(level);
		}
		break;
	case ']': if(cdrop().type!=cif) panic("bad branch"); break;
	case '^':
		if(drop()==0)
		{
			if(cdrop().type!=cloop) panic("bad loop");
			skipc('(', ')');
		}
		break;
	case '_': push(st[drop()]); break;
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
	case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
	case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
	case 'y': case 'z':
		push(drop()*26 + thisc-'a'); break;
	case '{':{
		int c = getc(stdin);
		push(c==EOF ? 0 : c);
	}break;
	case '|':
		if(cdrop().type!=cif) panic("bad branch");
		skipc('[', ']');
		break;
	case '}': putchar(drop()); break;
	default: panic("unknown symbol");
	}
	return 0; /* this part is not reached */
}
