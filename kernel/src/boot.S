
.text

.global main
main:
	di
	ehb
	addi $a0, $sp, 0
	jal startup
hang:
	j hang

