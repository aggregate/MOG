	.file	1 "fact.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 1
	.abicalls

	.comm	IPROC,4,4
	.globl	ten
	.data
	.align	2
	.type	ten, @object
	.size	ten, 4
ten:
	.word	3
	.globl	a
	.align	2
	.type	a, @object
	.size	a, 4
a:
	.word	1109917696
	.text
	.align	2
	.globl	fact
	.set	nomips16
	.ent	fact
	.type	fact, @function
fact:
	.frame	$sp,32,$31		# vars= 0, regs= 1/0, args= 16, gp= 8
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$4,32($sp)
	lw	$2,32($sp)
	nop
	slt	$2,$2,2
	beq	$2,$0,$L2
	nop

	li	$2,1			# 0x1
	.option	pic0
	j	$L3
	nop

	.option	pic2
$L2:
	lw	$2,32($sp)
	nop
	addiu	$2,$2,-1
	move	$4,$2
	.option	pic0
	jal	fact
	nop

	.option	pic2
	move	$3,$2
	lw	$2,32($sp)
	nop
	mult	$3,$2
	mflo	$2
$L3:
	lw	$31,28($sp)
	addiu	$sp,$sp,32
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	fact
	.size	fact, .-fact
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function
main:
	.frame	$sp,40,$31		# vars= 8, regs= 1/0, args= 16, gp= 8
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	.cprestore	16
	sw	$0,24($sp)
	.option	pic0
	j	$L5
	nop

	.option	pic2
$L6:
	lw	$2,%got(IPROC)($28)
	nop
	lw	$3,0($2)
	lw	$2,24($sp)
	nop
	xor	$3,$3,$2
	lui	$2,%hi(ten)
	lw	$2,%lo(ten)($2)
	nop
	bne	$2,$0,1f
	div	$0,$3,$2
	break	7
1:
	mfhi	$2
	move	$4,$2
	.option	pic0
	jal	fact
	nop

	.option	pic2
	lw	$28,16($sp)
	mtc1	$2,$f0
	nop
	cvt.s.w	$f0,$f0
	mfc1	$3,$f0
	lui	$2,%hi(a)
	sw	$3,%lo(a)($2)
	lw	$2,24($sp)
	nop
	addiu	$2,$2,1
	sw	$2,24($sp)
$L5:
	lw	$2,24($sp)
	nop
	slt	$2,$2,2
	bne	$2,$0,$L6
	nop

	lw	$31,36($sp)
	addiu	$sp,$sp,40
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.ident	"GCC: (GNU) 4.7.2 20121105 (Red Hat 4.7.2-2.aa.20121114svn)"
