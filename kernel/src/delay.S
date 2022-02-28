
.section .text
.set noreorder

.global delay
delay:
	bgtz	$a0, delay
	addi	$a0, $a0, -1
	jr	$ra

