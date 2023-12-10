.data

.text
.global __start

__start:
  # Exit.

  li $t0, 25

  add $t1, $t0, $0

  move $a0, $zero
  dli $v0, 5058
  syscall
