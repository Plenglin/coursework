li x10, 0x11008000
li x11, 0x1100c000
loop:
lw x12, 0(x10)
addi x12, x12, 1
sw x12, 0(x11)
j loop