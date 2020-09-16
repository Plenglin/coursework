.data
	input: .word 0x10, 0x20, 0x30
    output: .byte 0, 0, 0
.text
test:
	la x15, input
    la x20, output
    li x25, 3
    
    call subroutine
stop:
	j stop

subroutine:
    addi sp, sp, -16
    sw x15, 0(sp)
    sw x20, 4(sp)
    sw x25, 8(sp)
    sw t0, 12(sp)
loop:
    beq x25, zero, end
    
    lw t0, 0(x15)
    sb t0, 0(x20)
    
    addi x15, x15, 4
    addi x20, x20, 1
    addi x25, x25, -1
    j loop
end:
    addi sp, sp, -16
    lw x15, 0(sp)
    lw x20, 4(sp)
    lw x25, 8(sp)
    lw t0, 12(sp)
    ret
