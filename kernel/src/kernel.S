
# setjmp/longjmp

.section .text
.global setjmp
setjmp:
	sw	$sp,  0*4($a0)
	sw	$ra,  1*4($a0)
	sw	$gp,  2*4($a0)
	sw	$s0,  3*4($a0)
	sw	$s1,  4*4($a0)
	sw	$s2,  5*4($a0)
	sw	$s3,  6*4($a0)
	sw	$s4,  7*4($a0)
	sw	$s5,  8*4($a0)
	sw	$s6,  9*4($a0)
	sw	$s7, 10*4($a0)
	sw	$s8, 11*4($a0)
	addi	$v0, $zero, 0
	jr	$ra

.global longjmp
longjmp:
	lw	$sp,  0*4($a0)
	lw	$ra,  1*4($a0)
	lw	$gp,  2*4($a0)
	lw	$s0,  3*4($a0)
	lw	$s1,  4*4($a0)
	lw	$s2,  5*4($a0)
	lw	$s3,  6*4($a0)
	lw	$s4,  7*4($a0)
	lw	$s5,  8*4($a0)
	lw	$s6,  9*4($a0)
	lw	$s7, 10*4($a0)
	lw	$s8, 11*4($a0)
	sw	$a1, 12*4($a0)
	addi	$v0, $zero, 1
	jr	$ra

.global eret_longjmp
eret_longjmp:
	lw	$sp,  0*4($a0)
	lw	$ra,  1*4($a0)
	lw	$gp,  2*4($a0)
	lw	$s0,  3*4($a0)
	lw	$s1,  4*4($a0)
	lw	$s2,  5*4($a0)
	lw	$s3,  6*4($a0)
	lw	$s4,  7*4($a0)
	lw	$s5,  8*4($a0)
	lw	$s6,  9*4($a0)
	lw	$s7, 10*4($a0)
	lw	$s8, 11*4($a0)
	lw	$s8, 11*4($a1)
	sw	$a1, 12*4($a0)
	mfc0	$a1, $14, 0	// save epc in context
	sw	$a1, 13*4($a0)
	mtc0	$ra, $14, 0	// store return address in epc
	addi	$v0, $zero, 1   // set return value
	ehb
	eret // jump back to return address, clearing stuff

