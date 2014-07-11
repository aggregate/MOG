	.file	1 "test.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 1
	.abicalls

	.comm	NSYSARGS,4,4

	.comm	IPROC,4,4

	.comm	a,4,4

	.comm	b,4,4
	.text
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function
main:
	.frame	$sp,32,$31		# vars= 0, regs= 1/0, args= 16, gp= 8
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	.cprestore	16
	.option	pic0
	jal	getchar
	nop

	.option	pic2
	lw	$28,16($sp)
	move	$3,$2
	lw	$2,%got(a)($28)
	nop
	sw	$3,0($2)
	lw	$2,%got(a)($28)
	nop
	lw	$2,0($2)
	nop
	sll	$2,$2,24
	sra	$2,$2,24
	move	$4,$2
	.option	pic0
	jal	putchar
	nop

	.option	pic2
	lw	$28,16($sp)
	move	$4,$0
	.option	pic0
	jal	exit
	nop

	.option	pic2
	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.align	2
	.globl	exit
	.set	nomips16
	.ent	exit
	.type	exit, @function
exit:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	sw	$4,0($sp)
	lw	$2,%got(NSYSARGS)($28)
	li	$3,16			# 0x10
	sw	$3,0($2)
	li	$2,16			# 0x10
	sw	$0,0($2)
	li	$2,20			# 0x14
	lw	$3,0($sp)
	#nop
	sw	$3,0($2)
	li	$2,24			# 0x18
	li	$3,555			# 0x22b
	sw	$3,0($2)
#APP
 # 8 "mog_syscall.c" 1
	sys 0
 # 0 "" 2
#NO_APP
	j	$31
	.end	exit
	.size	exit, .-exit
	.align	2
	.globl	time
	.set	nomips16
	.ent	time
	.type	time, @function
time:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	sw	$4,0($sp)
	lw	$2,%got(NSYSARGS)($28)
	li	$3,16			# 0x10
	sw	$3,0($2)
	li	$2,16			# 0x10
	li	$3,1			# 0x1
	sw	$3,0($2)
	li	$2,32			# 0x20
	lw	$3,0($sp)
	#nop
	sw	$3,0($2)
#APP
 # 17 "mog_syscall.c" 1
	sys 0
 # 0 "" 2
#NO_APP
	li	$2,16			# 0x10
	lw	$2,0($2)
	j	$31
	.end	time
	.size	time, .-time
	.align	2
	.globl	dup
	.set	nomips16
	.ent	dup
	.type	dup, @function
dup:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	sw	$4,0($sp)
	lw	$2,%got(NSYSARGS)($28)
	li	$3,8			# 0x8
	sw	$3,0($2)
	li	$2,16			# 0x10
	li	$3,2			# 0x2
	sw	$3,0($2)
	li	$2,20			# 0x14
	lw	$3,0($sp)
	#nop
	sw	$3,0($2)
	li	$2,24			# 0x18
	li	$3,333			# 0x14d
	sw	$3,0($2)
#APP
 # 29 "mog_syscall.c" 1
	sys 0
 # 0 "" 2
#NO_APP
	li	$2,16			# 0x10
	lw	$2,0($2)
	j	$31
	.end	dup
	.size	dup, .-dup
	.align	2
	.globl	putchar
	.set	nomips16
	.ent	putchar
	.type	putchar, @function
putchar:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$2,$4
	sb	$2,0($sp)
	li	$2,16			# 0x10
	li	$3,3			# 0x3
	sw	$3,0($2)
	li	$2,20			# 0x14
	lb	$3,0($sp)
	#nop
	sw	$3,0($2)
#APP
 # 39 "mog_syscall.c" 1
	sys 0
 # 0 "" 2
#NO_APP
	j	$31
	.end	putchar
	.size	putchar, .-putchar
	.align	2
	.globl	getchar
	.set	nomips16
	.ent	getchar
	.type	getchar, @function
getchar:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	li	$2,16			# 0x10
	li	$3,4			# 0x4
	sw	$3,0($2)
#APP
 # 46 "mog_syscall.c" 1
	sys 0
 # 0 "" 2
#NO_APP
	li	$2,16			# 0x10
	lw	$2,0($2)
	j	$31
	.end	getchar
	.size	getchar, .-getchar
	.ident	"GCC: (GNU) 4.7.2 20121105 (Red Hat 4.7.2-2.aa.20121114svn)"
