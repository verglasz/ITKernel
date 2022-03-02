

.section .start,"ax",@progbits
.global _start

# we might want to zero out .bss or something like that?

_start:
	addi	$a0, $zero, 0
	addi	$a1, $zero, 0 # just in case anybody was silly enough to use argc/argv
	addi	$sp, $sp, -16 # just in case those same people saved args on the stack
	sw	$s8, 8($sp)   # why not try and
	sw	$ra, 12($sp)  # save some values
	addi	$s8, $sp, 0   # save stack in s8 to protect from stack pointer getting entirely mangled

	jal main
	nop

	addi	$a0, $v0, 0	# use return value of main as status code for exit

	jal exit
	nop

	addi	$sp, $s8, 0   # if we're here, something's very wrong
	lw	$s8, 8($sp)   # why not try and
	lw	$ra, 12($sp)  # restore some values

	addi	$sp, $sp, 16

	jr $ra	# return... unclear to what
	nop

