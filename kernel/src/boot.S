
.text
.set noreorder

.global main
main:
	di
	ehb
	jal startup
	addi $a0, $sp, 0
hang:
	j hang
	nop

