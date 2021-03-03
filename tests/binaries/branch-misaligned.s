.globl __start

.text
__start:
  addi x1, x1, 255
  beq x0, x0, -2
  addi x1, x1, 1023