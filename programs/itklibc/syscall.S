

.section .text
.global syscall

# all the callee-saved regs will be saved by the handler,
# all the caller-saved regs have been saved by the caller,
# the args are in their proper place,
# we're ready to invoke the kernel

syscall:
	syscall
	nop
	jr	$ra
	nop

