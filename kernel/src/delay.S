
.section .text

.global delay
delay:
	bgtz	$a0, delay
	nop
	jr	$ra

