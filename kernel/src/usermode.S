
# custom code in custom section for jumping into user mode

# we need to be in a memory area that allows user execution when we do this

.section .usermode,"ax",@progbits

# noreturn void usermode_jump(void (*jump_target)(void), usize * stack_pointer);
.global usermode_jump
usermode_jump:

	addi	$sp, $sp, -8
	sw	$s8, 0($sp)
	sw	$ra, 4($sp)
	addi	$s8, $sp, 0

	# set usermode
	mfc0	$t0, $12, 0
	ori	$t0, $t0, 0x10
	mtc0	$t0, $12, 0
	ehb

	addi	$sp, $a1, 0 # maaaybe we should check for alignment or something

	jalr	$a0	# here too
	nop

	# we should never get here
	addi	$sp, $s8, 0
	lw	$ra, 4($sp) # this will likely fail, since it's the kernel stack and we're user mode
	jr	$ra
	nop

.global nonusermode_jump
nonusermode_jump:
	addi	$sp, $sp, -8
	sw	$s8, 0($sp)
	sw	$ra, 4($sp)
	addi	$s8, $sp, 0

	# don't set usermode

	addi	$sp, $a1, 0 # maaaybe we should check for alignment or something

	jalr	$a0	# here too
	nop

	# we should never get here
	addi	$sp, $s8, 0
	lw	$ra, 4($sp) # this will likely fail, since it's the kernel stack and we're user mode
	jr	$ra
	nop

