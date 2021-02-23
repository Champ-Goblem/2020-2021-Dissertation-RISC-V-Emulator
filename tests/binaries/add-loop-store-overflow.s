.globl __start

.text
__start:
  addi x31, x31, 40
loop:
  addi x1, x1, 1
  sb x1, 0(x31)
  jal x2, loop