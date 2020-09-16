.data
foo: .word 32
.text 
li sp, 0x4000
li t2, 32
sw t2, 0(sp)
lw t0, 0(sp)