.globl __start

.text
__start:
  addi x2, x2, 1
  beq x0, x2, -2
  addi x1, x1, 1023