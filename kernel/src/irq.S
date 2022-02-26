
# General note for .section options:
# "ax" - allocatable, executable
# @progbits - section contains data
# without "ax",@progbits, our secttions would not be allocated in
# memory as they don't have a standard name

.set noreorder	# don't insert `nop`s, explicitely handle hazards
.set noat	# don't use $at (we want full control over the registers)

# NOTE: in Single Vector Mode, all IRQ will be forwarded
# to __vector_0
.altmacro

.macro isr_loader_default idx
	.align 4
	# the `vector_(new)_idx` sections are used by the linker
	# to place the handlers in the proper spot
	.section .vector_new_\idx,"ax",@progbits
	.global __vector_\idx
	__vector_\idx:
		addi	$k1, $zero, \idx # save interrupt number
		la	$k0, _default_isr
		jr	$k0
		nop
.endm


# ISR loaders
.align 4
.global _isr_loaders
_isr_loaders:
.set	i, 0
.rept	64
	isr_loader_default	%i
	.set			i, i+1
.endr

# Default ISR
.align 4
.global _default_isr
_default_isr:

	# from what i understand we should have a shadow register set,
	# but let's just save everything to be sure
	addi	$sp, $sp, -72
	sw	$ra,  0($sp)
	sw	$at,  4($sp)

	sw	$v0,  8($sp)
	sw	$v1, 12($sp)

	sw	$a0, 16($sp)
	sw	$a1, 20($sp)
	sw	$a2, 24($sp)
	sw	$a3, 28($sp)

	sw	$t0, 32($sp)
	sw	$t1, 36($sp)
	sw	$t2, 40($sp)
	sw	$t3, 44($sp)
	sw	$t4, 48($sp)
	sw	$t5, 52($sp)
	sw	$t6, 56($sp)
	sw	$t7, 60($sp)
	sw	$t8, 64($sp)
	sw	$t9, 68($sp)

	addi	$a0, $k1, 0	# use interrupt number as argument
	jal	default_isr
	nop

	lw	$t9, 68($sp)
	lw	$t8, 64($sp)
	lw	$t7, 60($sp)
	lw	$t6, 56($sp)
	lw	$t5, 52($sp)
	lw	$t4, 48($sp)
	lw	$t3, 44($sp)
	lw	$t2, 40($sp)
	lw	$t1, 36($sp)
	lw	$t0, 32($sp)

	lw	$a3, 28($sp)
	lw	$a2, 24($sp)
	lw	$a1, 20($sp)
	lw	$a0, 16($sp)

	lw	$v1, 12($sp)
	lw	$v0,  8($sp)

	lw	$at,  4($sp)
	lw	$ra,  0($sp)
	addi	$sp, $sp, 72

	eret
	nop


# Handle exceptions (traps, syscalls, etc)
# for explanation of "ax",@progbits see isr_stub
.section .gen_handler,"ax",@progbits
.ent _gen_exception
_gen_exception:
	addi	$k0, $ra, 0
	addi	$k1, $sp, 0

	# Use kernel's stack
	lw		$sp, sp_savemem

	# Call C exception handler
	jal	general_exception
	nop

	# Go back to user's stack
	addi	$sp, $k1, 0
	addi	$ra, $k0, 0

	# Go to next instruction
	mfc0	$k0, $14, 0
	ehb
	addi	$k0, $k0, 4
	mtc0	$k0, $14, 0
	ehb

	eret
	# Technically eret has no delay slot, so this shouldn't be needed
	nop
.end _gen_exception

