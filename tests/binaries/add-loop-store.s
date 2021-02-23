.globl __start

.text
__start:
  addi x4, x4, 30
loop:
  addi x1, x1, 1
  sw x1, 0(x4)
  jal x2, loop