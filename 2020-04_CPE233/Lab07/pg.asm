# Test Code
	li x25, 0x65318945
    call sort
	li x25, 0x98558345
    call sort
	li x25, 0x87126354
    call sort
stop:
    j stop 

#---------------------------------------------------------------- 
#- Sorts the nibbles stored in a register using insertion sort.
#-
#- Parameters
#-      x25 - the nibbles to sort
#- Returns
#-      x25 - the sorted nibbles
#- Tweaked registers - x25
#----------------------------------------------------------------     
sort:
    addi sp, sp, -32
    sw x6, 0(sp)
    sw x7, 4(sp)
    sw x8, 8(sp)
    sw x9, 12(sp)
    sw x10, 16(sp)
    sw x11, 20(sp)
    sw x12, 24(sp)
    sw x13, 28(sp)

    andi x7, x25, 0xF   # x7 = output. Insert first element.
    srli x25, x25, 4    # Remove first element.
    li x12, 4           # Length of x7
    li x13, 32          # End at this value
    loop:
        beq x12, x13, end
        andi x9, x25, 0xF    # x9 = current unsorted element
        srli x25, x25, 4     # Move "read head" forwards
        li x10, 0            # Shift counter
        
        insertloop:    # Find the next element
            bge x10, x12, insertAtEnd   # if shift counter is at the end

            srl x6, x7, x10     # x6 = Shift sorted elements
            andi x8, x6, 0xF    # x8 = Current sorted element
            blt x8, x9, noinsert    # if sorted > unsorted
                slli x6, x6, 4      # Move everything > x8 1 nibble left
                or x6, x6, x9       # Place unsorted into this position
                sll x6, x6, x10     # Move back to actual position

                # Build mask for existing values
                li x11, -1
                sll x11, x11, x10 
                xori x11, x11, -1 # Toggle ALL bits
                and x7, x7, x11  # Mask existing values
                or x7, x7, x6    # Mask new values
                j endinsertloop
            noinsert:
                addi x10, x10, 4
                j insertloop

            insertAtEnd:         # Insert at the very end
                sll x9, x9, x12
                or x7, x7, x9
        endinsertloop:
            
        addi x12, x12, 4
        j loop
    end:
    mv x25, x7
    
    lw x6, 0(sp)
    lw x7, 4(sp)
    lw x8, 8(sp)
    lw x9, 12(sp)
    lw x10, 16(sp)
    lw x11, 20(sp)
    lw x12, 24(sp)
    lw x13, 28(sp)
    addi sp, sp, 32

    ret
