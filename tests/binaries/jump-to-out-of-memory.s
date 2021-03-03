.globl __start

.text
__start:
  addi x1, x1, 1000
  jalr x2, 0(x1)