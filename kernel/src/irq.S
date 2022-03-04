
# General note for .section options:
# "ax" - allocatable, executable
# @progbits - section contains data
# without "ax",@progbits, our secttions would not be allocated in
# memory as they don't have standard names

.set noreorder	# don't insert `nop`s, explicitely handle hazards
.set noat	# don't use $at (we want full control over the registers)

# NOTE: in Single Vector Mode, all IRQ will be forwarded
# to __vector_0

.altmacro
.macro isr_trampoline idx
	# the `vector_(new)_idx` sections are used by the linker
	# to place the handlers in the proper spot
	# for explanation of "ax",@progbits see top of file
	.section .vector_new_\idx,"ax",@progbits
	.global __vector_\idx
	__vector_\idx:
		addi	$k0, $zero, 1 # mark as interrupt
		addi	$k1, $zero, \idx # save interrupt number
		j	_trap_landing_pad
		nop
.endm


# ISR trampolines (asyncronous traps)
.set	i, 0
.rept	64
	isr_trampoline	%i
	.set		i, i+1
.endr

# Syncronous trap handler (exceptions, syscalls, etc)
# for explanation of "ax",@progbits see top of file
.section .gen_handler,"ax",@progbits
.ent _gen_exception
.global _gen_exception
_gen_exception:
	mfc0	$k1, $13, 0	# read Cause register
	andi	$k0, $k1, 0x7d	# mask out cause bits
	xori	$k0, $k0, 0x20	# compare to cause Syscall
	beqz	$k0, _syscall   # if it's a syscall don't go to main landing pad (no need to save regs)
	addi	$k0, $zero, 0   # mark as exception
	j	_trap_landing_pad
	nop
_syscall:
#	addi	$k0, $sp, 0
	# use kernel stack (syscalls always come from userspace)
#	lw	$sp, kernel_sp
	jal	syscall_handler
	nop
#	addi	$sp, $k0, 0	# restore user stack
	mfc0	$k0, $14, 0	# advance instruction pointer (skip syscall)
	addi	$k0, $k0, 4 # maybe we should check if we were in MIPS16e mode?
	# i think it's also legal per misp spec to have syscall in a delay slot,
	# but we just declare that we don't support user programs that do it
	mtc0	$k0, $14, 0
	ehb	# actually eret shoud clear the hazards itself?
	eret
	nop

.end _gen_exception


.section .text

.align 4
.global _cause_interrupt
_cause_interrupt:
	li	$k0, 1
	mtc0	$ra, $14, 0
	ehb
	nop
	nop
	b	_trap_landing_pad
	nop

.global _cause_exception
_cause_exception:
	li	$k0, 0
	mtc0	$ra, $14, 0
	ehb
	nop
	b	_trap_landing_pad
	nop

.align 4
.global _trap_landing_pad
_trap_landing_pad:
	# from what i understand we should have a shadow register set,
	# but let's just save everything to be sure and not worry about it

	# we save 18 registers of 4 bytes each (=72 bytes), and leave additional space for
	# the arguments of the function we call as per the calling convention
	# (it should only use one, but I can't understand if the stack needs to be
	# aligned to a multiple of 8 bytes or not so let's just be sure)
	addi	$sp, $sp, -80
	sw	$ra,  76($sp)
	sw	$at,  72($sp)

	sw	$t9, 68($sp)
	sw	$t8, 64($sp)
	sw	$t7, 60($sp)
	sw	$t6, 56($sp)
	sw	$t5, 52($sp)
	sw	$t4, 48($sp)
	sw	$t3, 44($sp)
	sw	$t2, 40($sp)
	sw	$t1, 36($sp)
	sw	$t0, 32($sp)

	sw	$a3, 28($sp)
	sw	$a2, 24($sp)
	sw	$a1, 20($sp)
	sw	$a0, 16($sp)

	sw	$v1, 12($sp)
	sw	$v0,  8($sp)

	addi	$at, $k0, 0	# caller-saved registers have been saved so we can use them
	addi	$a0, $k1, 0	# use interrupt number/cause as argument

#	mfc0	$k1, $12, 0	# read status register
#	andi	$k1, $k1, 0x10	# get User Mode bit
#	beqz	$k1, _was_kernel	# if we come from kernel mode keep going
#	addi	$k0, $sp, 0		# else if we were in user mode,
#	lw	$sp, kernel_sp		# switch to kernel stack
_was_kernel:
	beqz	$at, _exception
	nop
	jal	interrupt_handler
	nop
	b	_continue
	nop
_exception:
	jal	exception_handler
	nop

	# Go to next instruction
	mfc0	$k0, $14, 0
	addi	$k0, $k0, 4 # maybe we should check if we were in MIPS16e mode?
	# also i'm pretty sure we should check if we're in a delay slot...
	mtc0	$k0, $14, 0
	ehb

_continue:
#	addi	$sp, $k0, 0	# unconditionally restore the stack,
				# since at worst it's a no-op
	lw	$ra,  76($sp)
	lw	$at,  72($sp)

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

	addi	$sp, $sp, 80

	eret
	nop

