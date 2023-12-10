.data
fail_msg: .asciiz "\nYour number is not between 0 and 9"
success_msg: .asciiz "\nYour number is  between 0 and 9"

.text

.globl __start

__start:

    li $t0, 10
    li $t1, 9

    slt $t2, $t0, $zero
    bne $t2, $zero, fail
    
    slt $t2, $t1, $t0
    bne $t2, $zero, fail
    
    jal success


fail:
	dla $a0, fail_msg
	li $v0, 4
	syscall
	
	# Exit the program
        li $v0, 10
        syscall


success:
	dla $a0, success_msg
	li $v0, 4
	syscall
