#---------------------------------------------------------------- 
#- Determines the largest and smallest unsigned byte values in 
#- a given chunk of memory. Stores results in a given address.
#-
#- Parameters
#-      x30 - memory start location
#-      x10 - length of memory
#-      x8 - where to store results. Min is 1 word, followed by 
#-           max, another word.
#- Tweaked registers - None
#---------------------------------------------------------------- 
bcd:
    addi sp, sp, -20
    sw t0, 0(sp)        # Min
    sw t1, 4(sp)        # Max
    sw t2, 8(sp)        # RAM Value
    sw x10, 12(sp)
    sw x30, 16(sp)

    li t0, 255          # Min
    li t1, 0            # Max

loop:
    bgt x8, zero, end
    lbu t2, 0(x30)      # Load value into t2

    addi x30, x30, 1    # Increment address
    addi x10, x10, -1   # Decrement count

    bgt t2, t0, notmin  # Skip next if t2 is not min
    mv t0, t2           # If it is min, record it as such
notmin:
    blt t2, t1, loop    # Skip next if t2 is not max
    mv t1, t2           # If it is max, record it as such
    j loop

end:
    sw t0, 0(x8)
    sw t1, 4(x8)

    lw t0, 0(sp)        # Min
    lw t1, 4(sp)        # Max
    lw t2, 8(sp)        # RAM Value
    lw x10, 12(sp)
    lw x30, 16(sp)
    addi sp, sp, 20

    ret