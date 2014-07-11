#define	PRsys		{ printf("sys %d", REGNAME(ir)); }

#define	PRadd		{ printf("add $%d", REGNAME(ir)); }
#define	PRaddf		{ printf("addf $%d", REGNAME(ir)); }
#define	PRand		{ printf("and $%d", REGNAME(ir)); }
#define	PRdiv		{ printf("div $%d", REGNAME(ir)); }
#define	PRdivu		{ printf("divu $%d", REGNAME(ir)); }
#define	PRdivf		{ printf("divf $%d", REGNAME(ir)); }
#define	PRxor		{ printf("xor $%d", REGNAME(ir)); }
#define	PRmul		{ printf("mul $%d", REGNAME(ir)); }
#define	PRmulf		{ printf("mulf $%d", REGNAME(ir)); }
#define	PRor		{ printf("or $%d", REGNAME(ir)); }
#define	PRslt		{ printf("slt $%d", REGNAME(ir)); }
#define	PRsltu		{ printf("sltu $%d", REGNAME(ir)); }
#define	PRsltf		{ printf("sltf $%d", REGNAME(ir)); }
#define	PRneg		{ printf("neg $%d", REGNAME(ir)); }
#define	PRnegf		{ printf("negf $%d", REGNAME(ir)); }
#define	PRrem		{ printf("rem $%d", REGNAME(ir)); }
#define	PRremu		{ printf("remu $%d", REGNAME(ir)); }
#define	PRsra		{ printf("sra $%d", REGNAME(ir)); }
#define	PRsll		{ printf("sll $%d", REGNAME(ir)); }
#define	PRsrl		{ printf("srl $%d", REGNAME(ir)); }

#define	PRj		{ printf("j"); }
#define	PRjf		{ printf("jf 0x%x", CPOOL(pc, ir)); }
#define	PRjt		{ printf("jt 0x%x", CPOOL(pc, ir)); }

#define	PRi2f		{ printf("i2f"); }
#define	PRu2f		{ printf("u2f"); }
#define	PRf2i		{ printf("f2i"); }

#define	PRli		{ printf("li %d", CPOOL(pc, ir)); }
#define	PRlr		{ printf("lr $%d", REGNAME(ir)); }
#define	PRsr		{ printf("sr $%d", REGNAME(ir)); }
#define	PRlw		{ printf("lw"); }
#define	PRsw		{ printf("sw $%d", REGNAME(ir)); }
#define	PRlh		{ printf("lh"); }
#define	PRsh		{ printf("sh $%d", REGNAME(ir)); }
#define	PRlb		{ printf("lb"); }
#define	PRsb		{ printf("sb $%d", REGNAME(ir)); }
