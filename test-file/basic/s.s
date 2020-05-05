	.file	1 "s.c"
	.section .mdebug.abi32
	.previous
	.nan	legacy
	.module	fp=xx
	.module	nooddspreg
	.abicalls

	.comm	change,40,4
	.text
	.align	2
	.globl	cmp
	.set	nomips16
	.set	nomicromips
	.ent	cmp
	.type	cmp, @function
cmp:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$4,8($fp)
	sw	$5,12($fp)
	lw	$3,8($fp)
	lw	$2,12($fp)
	addu	$2,$3,$2
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	cmp
	.size	cmp, .-cmp
	.rdata
	.align	2
$LC0:
	.ascii	"false\000"
	.text
	.align	2
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$fp,32,$31		# vars= 0, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	.cprestore	16
	li	$5,2			# 0x2
	li	$4,1			# 0x1
	.option	pic0
	jal	cmp
	nop

	.option	pic2
	lw	$28,16($fp)
	lw	$2,%got(change)($28)
	li	$3,1			# 0x1
	sw	$3,4($2)
	lw	$2,%got(change)($28)
	li	$3,2			# 0x2
	sw	$3,12($2)
	lw	$2,%got(change)($28)
	lw	$3,4($2)
	lw	$2,%got(change)($28)
	lw	$2,12($2)
	addu	$3,$3,$2
	lw	$2,%got(change)($28)
	sw	$3,20($2)
	lw	$2,%got(change)($28)
	lw	$3,4($2)
	lw	$2,%got(change)($28)
	lw	$2,12($2)
	slt	$2,$3,$2
	beq	$2,$0,$L4
	nop

	lw	$2,%got(change)($28)
	lw	$2,20($2)
	move	$4,$2
	lw	$2,%call16(printf)($28)
	move	$25,$2
	.reloc	1f,R_MIPS_JALR,printf
1:	jalr	$25
	nop

	lw	$28,16($fp)
	b	$L6
	nop

$L4:
	lw	$2,%got(change)($28)
	lw	$2,20($2)
	move	$5,$2
	lui	$2,%hi($LC0)
	addiu	$4,$2,%lo($LC0)
	lw	$2,%call16(printf)($28)
	move	$25,$2
	.reloc	1f,R_MIPS_JALR,printf
1:	jalr	$25
	nop

	lw	$28,16($fp)
$L6:
	nop
	move	$sp,$fp
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addiu	$sp,$sp,32
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
