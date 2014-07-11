/*	mogas.c

	Simple assembler for MOG by H. Dietz
	First alpha test version, 20101122
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define	VERSION	20101122
#define	TARGET	"cuda"

/*	Number of registers in target machine */
#define	REGS		256
int	maxreg = -1;

/*	Opcode definitions */
#include "op.h"
#include "cost.h"

#define	WARPSIZE	32

#define	BYTESPEROP	1

/*	Token type definitions */
#define	WORD	'W'
#define	NUMBER	'N'
#define	MYEOF	'E'
#define	KEQU	'q'
#define	KSET	's'
#define	EQUVAR	'Q'
#define	SETVAR	'S'
#define	KCONST	'C'
#define	KIFDEF	'i'
#define	KENDIF	'e'
#define	KTEXT	'T'
#define	KDATA	'D'
#define	KVER	'V'
#define	KTARGET	't'
#define	KTNAME	'n'
#define	KWORD	'w'
#define	KSPACE	'P'

#define	RETS	'R'
#define	FUNC	'F'

#define	MAXLEX	32		/* Length of maximum lexeme */
#define	MAXFREE	(1024*1024)	/* Size of free char pool */
#define	MAXSYMS	1024		/* Size of symbol table */

int	nextt = ' ';
char	lexeme[MAXLEX];
int	lexval;
int	lexsym;
int	nextc = ' ';
int	lineno = 0;

typedef struct {
	int	lexdex;		/* Lexeme index in freec */
	int	typ;		/* Token type */
	int	val;		/* Token value */
} symtyp;

char	freec[MAXFREE];
int	freecsp = 0;
symtyp	symtab[MAXSYMS];
int	symsp = 0;

int	labno = 0;
int	sp = 0;
int	args = 0;

char	inputbuf[MAXFREE*4];
int	inputend = 0;
int	inputx = 0;
int	pass = 1;
int	nocode = 0;

#define	LC	(lc[seg])
int	lc[2] = { 0, 0 };
int	seg = 0;

#define	MAXADDR	0x000fffff
int	segwords[2][MAXADDR];
int	segmax[2] = { -1, -1 };

int	targetok = 0;		/* Is code for this target? */

/*	Opcode use summary */
#define	O2X(X)	(OPCODE(X)>>8)
float	freq[255+1];
float	prob[255+1][255+1];
#define	MAXRETS	1024
int	retstab[MAXRETS];
int	retsp = 0;


/*	Error/warning stuff
*/

void
warn(char *mesg)
{
	fprintf(stderr, "Warning near line %d: %s\n", lineno, mesg);
}

void
error(char *mesg)
{
	fprintf(stderr, "Error near line %d: %s\n", lineno, mesg);
	exit(1);
}



/*	Constant pool stuff
*/

#define	CPOOLSIZE cpoolsize
#define	MAXCPOOL 0x000fffff
struct {
	int val, used;
} cpool[MAXCPOOL];
int	cpoolsize = MAXCPOOL;
int	cpoolcount = MAXCPOOL;


void
clearcpool(void)
{
	register int i;

	/* Resize cpool to power-of-2 >= entry count */
	cpoolsize = cpoolcount;;
	while (cpoolsize & (cpoolsize - 1)) ++cpoolsize;

	for (i=0; i<MAXCPOOL; ++i) {
		cpool[i].used = 0;
	}
	cpoolcount = 0;
}

int
mkcpool(register int mylc,
register int myop,
register int myval)
{
	register int i, h;

	/* Count number of constants... */
	++cpoolcount;

	/* Look for a duplicate entry... */
	for (i=0; i<=0xff; ++i) {
		h = CHASH(mylc, (myop + i));
		if ((cpool[h].used != 0) &&
		    (cpool[h].val == myval)) {
			/* Found it! */
#ifdef	DEBUG
			if (pass == 2) {
				fprintf(stderr, "%d ref to constant %d in pool\n", cpool[h].used, myval);
				++(cpool[h].used);
			}
#endif
			return(myop + i);
		}
	}

	/* Look for an empty entry... */
	for (i=0; i<=0xff; ++i) {
		h = CHASH(mylc, (myop + i));
		if (cpool[h].used == 0) {
			cpool[h].val = myval;
			cpool[h].used = 1;
			return(myop + i);
		}
	}

	if (pass == 2) {
		error("Unable to allocate constant");
	}
}

void
dumpcpool(void)
{
	register int i, j;

	printf("\n#define CPOOLSIZE %d\n", cpoolsize);
	printf("int cpool[CPOOLSIZE] = {\n");
	for (i=0; i<cpoolsize; ++i) {
		j = (cpool[i].used ? cpool[i].val : 0);
		printf("\t%d", j);
		printf((i+1 != cpoolsize) ? "," : "\n};");
		if ((i & 7) == 7) printf("\n");
	}
	printf("\n");
}


void
inituse(void)
{
	register int i, j;

	for (j=0; j<=255; ++j) {
		for (i=0; i<=255; ++i) {
			prob[j][i] = 0;
		}
	}
	for (i=0; i<=255; ++i) {
		freq[i] = 0;
	}
}

inline static void
retsuse(register int a)
{
	if (pass == 2) retstab[retsp++] = a;
}

void
traceuse(void)
{
	/* Trace use along all paths */
	register lastop = -1;
	register int i;

	/* Count return adjacencies as OPj ___ */
	for (i=0; i<retsp; ++i) {
		++(prob[O2X(OPj)][O2X(segwords[0][ retstab[i] ])]);
	}

	/* Now count from memory image */
	lastop = -1;
	for (i=0; i<segmax[0]; ++i) {
		register int x = O2X(segwords[0][i]);
		if (lastop != -1) {
			++(prob[lastop][x]);
		}
		++freq[x];
		switch (x) {
		case (OPj>>8):
			/* Do not fall through */
			lastop = -1;
			break;
		case (OPjf>>8):
		case (OPjt>>8):
			/* Add target */
			++(prob[x][O2X(segwords[0][ cpool[CHASH(i, segwords[0][i])].val ])]);
			break;
		default:
			lastop = x;
		}
	}
}

/*	Output stuff
*/

void
genword(register int v)
{
	if (nocode) return;

	/* Force alignment */
	if (seg) LC = (LC + 3) & ~3;

	if (pass == 2) {
		segwords[seg][LC] = v;
		if (LC > segmax[seg]) segmax[seg] = LC;
	}

	LC += (seg?4:1);
}

/*	Symbol table
*/

int
enter(char *lexeme,
int typ,
int val)
{
	if (symsp >= MAXSYMS) {
		error("No space in symbol table");
	}

	symtab[symsp].lexdex = freecsp;
	do {
		if (freecsp >= MAXFREE) {
			error("No space in string table");
		}
		freec[freecsp++] = *lexeme;
	} while (*(lexeme++));

	symtab[symsp].typ = typ;
	symtab[symsp].val = val;
	return(symsp++);
}

int
lookup(char *lexeme)
{
	register int i = symsp-1;

	while (i >= 0) {
		if (strcmp(lexeme, &(freec[ symtab[i].lexdex ])) == 0) {
			return(i);
		}
		--i;
	}

	return(enter(lexeme, WORD, 0));
}

void
dumpsyms(void)
{
	register int i;

	printf("/* Symbols in MOG user data and code */\n");
	for (i=0; i<symsp; ++i) if (symtab[i].typ == EQUVAR) {
		printf("#define MOGSYM_%s %d\n",
		       &(freec[ symtab[i].lexdex ]),
		       symtab[i].val);
	}
	printf("\n");
}

/*	Lexical analyzer
*/

int
advance(void)
{
	if (inputx < inputend) {
		nextc = inputbuf[inputx++];
		if (nextc == '\n') ++lineno;
	} else {
		nextc = EOF;
	}
	return(nextc);
}

int
lex(void)
{
more:
	/* At EOF? */
	if ((nextt == MYEOF) || (nextc == EOF)) {
		return(nextt = MYEOF);
	}

	/* Recognize non-tokens */
	if (nextc <= ' ') {
		advance();
		goto more;
	}
	if (nextc == ';') {
		while ((nextc != '\n') && (nextc != EOF)) {
			advance();
		}
		goto more;
	}

	/* Recognize a word */
	if (((nextc <= 'Z') && (nextc >= 'A')) ||
	    ((nextc <= 'z') && (nextc >= 'a')) ||
	    (nextc == '_') || (nextc == '.')) {
		register int i = 0;

		do {
			if (i < MAXLEX) {
				lexeme[i++] = nextc;
			}
			advance();
		} while (((nextc <= 'Z') && (nextc >= 'A')) ||
			 ((nextc <= 'z') && (nextc >= 'a')) ||
			 ((nextc <= '9') && (nextc >= '0')) ||
			 (nextc == '_') ||
			 (nextc == '.'));
		lexeme[i] = 0;
		nextt = WORD;
		if ((lexsym = lookup(&(lexeme[0]))) != -1) {
			nextt = symtab[lexsym].typ;
		}
		return(nextt);
	}

	/* Recognize a number */
	if (nextc == '0') {
		advance();
		switch (nextc) {
		case 'b':
		case 'B':
			/* Binary */
			advance();
			lexval = 0;
			while ((nextc <= '1') && (nextc >= '0')) {
				lexval *= 2;
				lexval += (nextc - '0');
				advance();
			}
			return(nextt = NUMBER);
		case 'x':
		case 'X':
			/* Hex */
			advance();
			lexval = 0;
			while (((nextc <= '9') && (nextc >= '0')) ||
			       ((nextc <= 'f') && (nextc >= 'a')) ||
			       ((nextc <= 'F') && (nextc >= 'A'))) {
				lexval *= 16;
				if (nextc <= '9') lexval += (nextc - '0');
				else if (nextc <= 'F') lexval += (10 + nextc - 'A');
				else if (nextc <= 'f') lexval += (10 + nextc - 'a');
				advance();
			}
			return(nextt = NUMBER);
		case 'o':
		case 'O':
			advance();
		default:
			/* Octal */
			lexval = 0;
			while ((nextc <= '7') && (nextc >= '0')) {
				lexval *= 8;
				lexval += (nextc - '0');
				advance();
			}
			return(nextt = NUMBER);
		}
	}
	if ((nextc <= '9') && (nextc >= '0')) {
		/* Decimal */
		lexval = 0;
		do {
			lexval *= 10;
			lexval += (nextc - '0');
			advance();
		} while ((nextc <= '9') && (nextc >= '0'));
		return(nextt = NUMBER);
	}

	/* Punctuation is all that's left */
	nextt = nextc;
	advance();
	return(nextt);
}

/*	SWAGAC Parser
*/

int
match(register int t)
{
	if (nextt == t) {
		lex();
		return(1);
	}
	return(0);
}

extern unsigned int expr(void);
extern void stat(void);

unsigned int
expr4(void)
{
	register unsigned int t;

	switch (nextt) {
	case NUMBER:
		t = lexval;
		lex();
		return(t);
	case EQUVAR:
	case SETVAR:
		t = lexsym;
		lex();
		return(symtab[t].val);
	case WORD:
		/* Really shouldn't happen on pass 2 */
		if (
		pass == 2) {
			error("Undefined symbol");
		}
		lex();
		return(0);
	}

	if (match('-')) {
		return(-expr4());
	} else if (match('!')) {
		return(-!expr4());
	} else if (match('~')) {
		return(~expr4());
	} else if (match('(')) {
		t = expr();
		if (!match(')')) warn("missing ) assumed");
		return(t);
	}

	error("ill-formed expression");
}

unsigned int
expr3(void)
{
	register unsigned int t = expr4();
	while (match('*')) {
		t *= expr4();
	}
	return(t);
}

unsigned int
expr2(void)
{
	register unsigned int t = expr3();
	while ((nextt == '+') || (nextt == '-')) {
		register int n = nextt;

		lex();
		switch (n) {
		case '+': t += expr3(); break;
		case '-': t -= expr3(); break;
		}
	}
	return(t);
}

unsigned int
expr1(void)
{
	register unsigned int t = expr2();
	while ((nextt == '<') || (nextt == '>')) {
		register int n = nextt;

		lex();
		switch (t) {
		case '<': t = (t < expr2()); break;
		case '>': t = (t > expr2()); break;
		}
	}
	return(t);
}

unsigned int
expr(void)
{
	register unsigned int t = expr1();
	while ((nextt == '&') || (nextt == '|') || (nextt == '^')) {
		register int n = nextt;

		lex();
		switch (n) {
		case '&': t &= expr1(); break;
		case '|': t |= expr1(); break;
		case '^': t ^= expr1(); break;
		}
	}
	return(t);
}

unsigned int
reg(void)
{
	register unsigned int t;

	if (nextt == '$') lex();
	t = expr();

	if ((pass == 2) && ((t < 0) || (t >= REGS))) {
		warn("Illegal register number");
	}
	t %= REGS;
	if (((int)t) > maxreg) maxreg = t;
	return(t);
}

void
labstat(register int sym)
{
	register unsigned int v;

	if (match(KEQU)) {
		v = expr();
		if ((pass == 2) && (symtab[sym].val != v)) {
			warn("Redefinition of EQU symbol");
		}
		if (nocode == 0) {
			symtab[sym].val = v;
			symtab[sym].typ = EQUVAR;
		}
	} else if (match(KSET)) {
		if (nocode == 0) {
			symtab[sym].val = expr();
			symtab[sym].typ = SETVAR;
		} else {
			expr();
		}
	} else {
		if (!match(':')) warn("Missing : assumed");
		if (nocode == 0) {
			symtab[sym].val = LC;
			symtab[sym].typ = EQUVAR;
		}
		stat();
	}
}

void
stat(void)
{
	register int i, j;

	switch (j = nextt) {
	case KWORD:
		lex();
		genword(expr());
		break;
	case KSPACE:
		lex();
		i = expr();
		while (i > 0) {
			genword(0);
			--i;
		}
		break;
	case OPsys:
		lex();
		i = expr();
		if (i & ~0xff) error("illegal sys call number");
		genword(j + (i & 0xff));
		break;
	case OPli:
	case OPjf:
	case OPjt:
		lex();
		i = expr();
		genword(mkcpool(LC, j, i));
		break;
	case OPadd:
	case OPaddf:
	case OPand:
	case OPdiv:
	case OPdivu:
	case OPdivf:
	case OPxor:
	case OPmul:
	case OPmulf:
	case OPor:
	case OPslt:
	case OPsltu:
	case OPsltf:
	case OPneg:
	case OPnegf:
	case OPrem:
	case OPremu:
	case OPsra:
	case OPsll:
	case OPsrl:
	case OPlr:
	case OPsr:
	case OPsw:
	case OPsh:
	case OPsb:
		lex();
		genword(j + reg());
		break;
	case OPi2f:
	case OPu2f:
	case OPf2i:
	case OPj:
	case OPlw:
	case OPlh:
	case OPlb:
		lex();
		genword(j);
		break;
	case RETS:
		/* Return spec; ignore func spec */
		lex();
		retsuse(expr());
		break;
	case FUNC:
		/* Ignore func spec.... */
		lex();
		lex();
		break;
	case KIFDEF:
		lex();
		if ((symtab[lexsym].typ == EQUVAR) ||
		    (symtab[lexsym].typ == SETVAR)) {
			/* Assembly stays on, unless off now */
			lex();
			if (nocode) ++nocode;
		} else if (symtab[lexsym].typ == WORD) {
			/* Assembly off */
			lex();
			++nocode;
		} else {
			error(".ifdef missing symbol");
		}
		break;
	case KENDIF:
		lex();
		if (nocode) --nocode;
		break;
	case KTEXT:
		lex();
		seg = 0;
		break;
	case KDATA:
		lex();
		seg = 1;
		break;
	case KVER:
		lex();
		i = expr();
		if (i != VERSION) error("MOG environment version mismatch");
		break;
	case KTARGET:
		lex();
		if (nextt != KTNAME) error("MOG environment target is not " TARGET);
		lex();
		break;
	case WORD:
	case EQUVAR:
	case SETVAR:
		i = lexsym;
		lex();
		labstat(i);
		break;
	default:
		lex();
		warn("Unknown assembler directive or instruction ignored");
	}
}

void
prog(void)
{
	while (nextt != MYEOF) stat();
}

/*	Main
*/

void
dopass(register int p)
{
	inputx = 0;
	pass = p;
	nocode = 0;
	lc[0] = 0;
	lc[1] = 0;
	seg = 0;
	lineno = 0;
	nextt = (nextc = ' ');
	lex();
	prog();
}


/*	MAX number of single-instruction interpreters */
#define	MAX	32

/*	TIMEOUT for interpreter order search */
#define	TIMEOUT	(2)
int	timeout = TIMEOUT;

struct {
	char	*op;
	float	freq;
	float	cost;
	int	val;
} inst[256];

int	isp = 0;

void
initinst(void)
{
	/* Initialize instructions used data structure...
	   used to create optimized interpreter order
	*/

#define	INST(o) \
	if (freq[OP##o >> 8] > 0) { \
		inst[isp].op = #o; \
		inst[isp].freq = freq[OP##o >> 8]; \
		inst[isp].cost = COST##o; \
		inst[isp].val = OP##o >> 8; \
		++isp; \
	}

	INST(add)
	INST(addf)
	INST(and)
	INST(div)
	INST(divu)
	INST(divf)
	INST(xor)
	INST(mul)
	INST(mulf)
	INST(or)
	INST(slt)
	INST(sltu)
	INST(sltf)
	INST(neg)
	INST(negf)
	INST(rem)
	INST(remu)
	INST(sra)
	INST(sll)
	INST(srl)
	INST(j)
	INST(jf)
	INST(jt)
	INST(i2f)
	INST(u2f)
	INST(f2i)
	INST(li)
	INST(lr)
	INST(sr)
	INST(lw)
	INST(sw)
	INST(lh)
	INST(sh)
	INST(lb)
	INST(sb)
}


struct {
	double	bias;
	int	left, right;
} node[1024];

int	heap[1024];

int donum;


#define	MAXOPS	256

int	quota[MAXOPS];
int	order[MAX];
int	bestorder[MAX];


void
oneorder(void)
{
	/* Search for optimal order of single-instruction interpreters...
	   greatly simplified from previous version, but sloppy code
	*/
	register float total = 0;
	register int need;
	register int i, j, k, m;
	register double merit;
	register double limit;
	register double best = 1E+20;
	register int domore = (timeout != 0);
	register int start;

	initinst();

	fprintf(stderr, "Ordering %d single-instruction interpreters:\n", MAX);
	fprintf(stderr, "Using frequency-weighted minimum distance between 2-tuples...\n");

	/* Rescale quotas, ensuring all happen once..
	   assumes total will be greater than MAX
	*/
	for (i=0; i<isp; ++i) {
		total += inst[i].freq;
	}
	need = (MAX - isp);
	for (i=0; i<isp; ++i) {
		quota[i] = 1 + ((((inst[i].freq - 1) * need) + (total / 2)) / total);
		need -= (quota[i] - 1);
		total -= inst[i].freq;
	}

	/* Now construct an order */
	k = 0;
	for (i=0; i<isp; ++i) {
		for (j=0; j<quota[i]; ++j) {
			order[k] = i;
			if (k < MAX) ++k;
		}
	}

	/* Find theoretical best (which might not be possible) */
	limit = 0;
	for (i=0; i<MAX; ++i) {
		for (j=0; j<isp; ++j) {
			limit += prob[ inst[order[i]].val ][ inst[j].val ];
		}
	}

	/* Search for a good order */
	srand(start = time(0));
	m = 0;
	do {
		/* Eval the current order, including wraparound */
		merit = 0;

		/* Metric based distance between pairs */
		{
			double dist[MAX];
			register int a, b;

			for (a=0; a<MAX; ++a) {
				for (b=0;  b<isp; ++b) dist[b] = MAX;
				for (b=(MAX-1);  b>0; --b) {
					dist[order[(a + b) % MAX]] = b;
				}
				for (b=0; b<isp; ++b) {
					if (prob[ inst[order[a]].val ][ inst[b].val ]) {
						merit += (dist[b] * prob[ inst[order[a]].val ][ inst[b].val ]);
						if (merit >= best) goto nope;
					}
				}
			}
		}
		fprintf(stderr, "New best order %d: weighted average span %lg\n", m, (merit / limit));

		best = merit;
		for (i=0; i<MAX; ++i) bestorder[i] = order[i];
nope:		;

		/* Every so often, reset to best so far */
		if ((rand() & 31) == 0) {
			for (i=0; i<MAX; ++i) order[i] = bestorder[i];
		}

		/* Muck with the order */
		do {
			i = (rand() % (MAX-1));
			if ((rand() & 64) &&
			    (prob[ inst[order[i]].val ][ inst[order[i+1]].val ] <
			     prob[ inst[order[i+1]].val ][ inst[order[i]].val ])) {
				k = order[i];
				order[i] = order[i+1];
				order[i+1] = k;
			} else {
				do {
					i = (rand() % MAX);
					j = (rand() % MAX);
				} while (i == j);
				k = order[i];
				order[i] = order[j];
				order[j] = k;
			}
		} while (rand() % MAX);

		++m;

		/* Check for timeout every so often... */
		if (((m & (1024 - 1)) == 0) && ((time(0) - start) > timeout)) domore = 0;
	} while (domore && best);

	/* Embed stats in output */
	printf("/*\tInterpreter sequence:\n"
	       "\tWeighted average span is %lg\n"
	       "*/\n",
	       (best / limit));

	/* Output the interpreter sequence */
	printf("#define OPORDER \\\n");
	for (i=0; i<(MAX-1); ++i) {
		printf("\tOPIS(%s) \\\n", inst[bestorder[i]].op);
	}
	printf("\tOPIS(%s)\n", inst[bestorder[MAX-1]].op);
}


int
main()
{
	register int i, j, c;

	enter(".equ", KEQU, 0);
	enter(".set", KSET, 0);
	enter(".ifdef", KIFDEF, 0);
	enter(".endif", KENDIF, 0);
	enter(".text", KTEXT, 0);
	enter(".data", KDATA, 0);
	enter(".version", KVER, 0);
	enter(".target", KTARGET, 0);
	enter(TARGET, KTNAME, 0);
	enter(".word", KWORD, 0);
	enter(".space", KSPACE, 0);

	enter("sys", OPsys, 0);
	enter("add", OPadd, 0);
	enter("addf", OPaddf, 0);
	enter("and", OPand, 0);
	enter("div", OPdiv, 0);
	enter("divu", OPdivu, 0);
	enter("divf", OPdivf, 0);
	enter("xor", OPxor, 0);
	enter("mul", OPmul, 0);
	enter("mulf", OPmulf, 0);
	enter("or", OPor, 0);
	enter("slt", OPslt, 0);
	enter("sltu", OPsltu, 0);
	enter("sltf", OPsltf, 0);
	enter("neg", OPneg, 0);
	enter("negf", OPnegf, 0);
	enter("rem", OPrem, 0);
	enter("remu", OPremu, 0);
	enter("sra", OPsra, 0);
	enter("sll", OPsll, 0);
	enter("srl", OPsrl, 0);
	enter("j", OPj, 0);
	enter("jf", OPjf, 0);
	enter("jt", OPjt, 0);
	enter("i2f", OPi2f, 0);
	enter("u2f", OPu2f, 0);
	enter("f2i", OPf2i, 0);
	enter("li", OPli, 0);
	enter("lr", OPlr, 0);
	enter("sr", OPsr, 0);
	enter("lw", OPlw, 0);
	enter("sw", OPsw, 0);
	enter("lh", OPlh, 0);
	enter("sh", OPsh, 0);
	enter("lb", OPlb, 0);
	enter("sb", OPsb, 0);

	/* Informational directives */
	enter(".ret", RETS, 0);
	enter(".ent", FUNC, 0);

	/* Buffer all the input */
	inputend = 0;
	while ((c = getchar()) != EOF) {
		inputbuf[inputend++] = c;
	}

	/* Wipe all memory */
	for (j=0; j<2; ++j) {
		for (i=0; i<=MAXADDR; ++i) {
			segwords[j][i] = 0;
		}
	}

	/* Pass 1 */
	clearcpool();
	dopass(1);

	/* Another Pass 1 */
	clearcpool();
	dopass(1);

	/* Pass 2 */
	inituse();
	dopass(2);

	/* Dump header info */
	printf("int version = %d;\n", VERSION);
	printf("char target[] = \"%s\";\n", TARGET);
	printf("#define REGSUSED %d\n", maxreg+1);
	printf("#define NSYSARGS %d\n\n", segwords[1][2]);

	/* Dump symbol table */
	dumpsyms();

	/* Dump the memory images */
	for (j=0; j<2; ++j) {
		register int last = segmax[j];

		if (j) {
			printf("\n#define DATASIZE %d\n", segmax[j]+1);
			printf("int dataseg[DATASIZE] = {\n");
		} else {
			printf("\ntypedef short inst_t;\n");
			printf("#define TEXTSIZE %d\n", segmax[j]+1);
			printf("inst_t textseg[TEXTSIZE] = {\n");
		}

		/* Dump segment, but trim trailing 0s...
		   this is mostly to avoid dumping 0s for data stack space
		*/
		while (segwords[j][last] == 0) --last;
		for (i=0; i<=last; ++i) {
			printf((j ? "\t%d" : "\t0x%04x"), segwords[j][i]);
			printf((i < last) ? "," : "\n};");
			if ((i & 7) == 7) printf("\n");
		}
		printf("\n");
	}
	dumpcpool();
	fflush(stdout);
	/* Generate optimized interpreter sequence */
	traceuse();
	oneorder();
	fflush(stdout);
}
