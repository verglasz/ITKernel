
.text
.set reorder

.global main
main:
	di
	addi $a0, $sp, 0
	jal startup
hang:
	j hang

