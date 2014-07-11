#define	DOsys		{ }

#define	DOadd		{ a.i += REGI(ir); NEXT }
#define	DOaddf		{ a.f += REGF(ir); NEXT }
#define	DOand		{ a.u &= REGU(ir); NEXT }
#define	DOdiv		{ a.i /= REGI(ir); NEXT }
#define	DOdivu		{ a.u /= REGU(ir); NEXT }
#define	DOdivf		{ a.f /= REGF(ir); NEXT }
#define	DOxor		{ a.u ^= REGU(ir); NEXT }
#define	DOmul		{ a.i *= REGI(ir); NEXT }
#define	DOmulf		{ a.f *= REGF(ir); NEXT }
#define	DOor		{ a.u |= REGU(ir); NEXT }
#define	DOslt		{ a.i = (a.i < REGI(ir)); NEXT }
#define	DOsltu		{ a.u = (a.u < REGU(ir)); NEXT }
#define	DOsltf		{ a.f = (a.f < REGF(ir)); NEXT }
#define	DOneg		{ a.i = -a.i; NEXT }	/* Not MIPS */
#define	DOnegf		{ a.f = -a.f; NEXT }	/* Not MIPS */
#define	DOrem		{ a.i %= REGI(ir); NEXT }
#define	DOremu		{ a.u %= REGU(ir); NEXT }
#define	DOsra		{ a.i >>= REGI(ir); NEXT }
#define	DOsll		{ a.u <<= REGU(ir); NEXT }
#define	DOsrl		{ a.u >>= REGU(ir); NEXT }

#define	DOj		{ pc = a.u; NEXT }
#define	DOjf		{ if (a.u == 0) pc = CPOOL(pc, ir); NEXT }	/* Not MIPS */
#define	DOjt		{ if (a.u != 0) pc = CPOOL(pc, ir); NEXT }	/* Not MIPS */

#define	DOi2f		{ a.f = a.i; NEXT }
#define	DOu2f		{ a.f = a.u; NEXT }
#define	DOf2i		{ a.i = a.f; NEXT }	/* Also f2u */

#define	DOli		{ a.i = CPOOL(pc, ir); NEXT }
#define	DOlr		{ a.i = REGI(ir); NEXT }	/* Not MIPS */
#define	DOsr		{ REGI(ir) = a.i; NEXT }	/* Not MIPS */
#define	DOlw		{ a.i = MEMI(a.i); NEXT }
#define	DOsw		{ MEMI(REGI(ir)) = a.i; NEXT }
#define	DOlh		{ a.i = MEMH(a.u); NEXT }
#define	DOsh		{ MEMH(REGI(ir)) = a.i; NEXT }
#define	DOlb		{ a.i = MEMB(a.u); NEXT }
#define	DOsb		{ MEMB(REGI(ir)) = a.i; NEXT }
