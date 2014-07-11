#define	OPCODE(X)	((X) & 0xff00)
#define	REGNAME(X)	((X) & 0x00ff)

#define	CHASH(LC,I)	(((LC) + (I)) & (CPOOLSIZE - 1))

#define	OPsys	0x0000

#define	OPadd	0x0100
#define	OPaddf	0x0200
#define	OPand	0x0300
#define	OPdiv	0x0400
#define	OPdivu	0x0500
#define	OPdivf	0x0600
#define	OPxor	0x0700
#define	OPmul	0x0800
#define	OPmulf	0x0900
#define	OPor	0x0a00
#define	OPslt	0x0b00
#define	OPsltu	0x0c00
#define	OPsltf	0x0d00
#define	OPneg	0x0e00
#define	OPnegf	0x0f00
#define	OPrem	0x1000
#define	OPremu	0x1100
#define	OPsra	0x1200
#define	OPsll	0x1300
#define	OPsrl	0x1400

#define	OPj	0x1500
#define	OPjf	0x1600
#define	OPjt	0x1700

#define	OPi2f	0x1800
#define	OPu2f	0x1900
#define	OPf2i	0x2000

#define	OPli	0x2100
#define	OPlr	0x2200
#define	OPsr	0x2300
#define	OPlw	0x2400
#define	OPsw	0x2500
#define	OPlh	0x2600
#define	OPsh	0x2700
#define	OPlb	0x2800
#define	OPsb	0x2900
