.data
    sseg: .space 10 # Save space for 10-byte lookup table
    bcdData: .space 1
    digitEncountered: .space 1
    digitIndex: .space 1

    pressedDuration: .space 4 

.eqv BTN_DURATION, 100

.text
main:
    # Load 7-segment data
    la s7, sseg
    li t0, 0x03
    sb t0, 0(s7)
    li t0, 0x9F
    sb t0, 1(s7)
    li t0, 0x25
    sb t0, 2(s7)
    li t0, 0x0D
    sb t0, 3(s7)
    li t0, 0x99
    sb t0, 4(s7)
    li t0, 0x49
    sb t0, 5(s7)
    li t0, 0x41
    sb t0, 6(s7)
    li t0, 0x1F
    sb t0, 7(s7)
    li t0, 0x01
    sb t0, 8(s7)
    li t0, 0x09
    sb t0, 9(s7)

    # Load initial digit encounter
    la t0, digitEncountered
    sb zero, 0(t0)

    # Load initial digit index
    la t0, digitIndex
    li t1, 3
    sb t1, 0(t0)

    # Load initial pressed duration
    la t0, pressedDuration
    li t1, BTN_DURATION
    sw t1, 0(t0)
    
    # Load initial stack pointer
    li sp, 0x4000

    li s0, 980                  # Pressed times count
    li s1, 0                    # "Redraw" flag
    li s3, 0                    # Cached BCD value

    # Set up TC
    li t1, 0x1100D004  # timer counter count port address
    li t0, 0x00FFFFFF  # for ~1.5Hz blink rate
    sw t0, 0(t1)      # init TC count

    li t1, 0x1100D000  # timer counter CSR port address
    li t0, 0x01        # init TC CSR
    sw t0, 0(t1)      # no prescale, turn on TC

    # Enable ISR
    la t0, isr
    csrrw x0, mtvec, t0 

doMainLoop:
    li t0, 1
    csrrw x0, mie, t0 
    
    # a2 = button pressed
    li t0, 0x11000000
    lw a2, 0(t0)
    andi a2, a2, 0x1

    call debounceStep

shouldIncrementCount:
    beq a0, zero, shouldDraw
incrementCount:
    addi s0, s0, 1
    mv a0, s0
    call bcd  # Recalculate BCD 
    mv s3, a1
shouldDraw:
    beq s1, zero, doMainLoop
    li s1, 0
    call draw
    j doMainLoop
isr:
    li s1, 1              # Set "redraw" flag
    mret


#----------------------------------------------------------------
#- Subroutine for debouncing and positive edges
#- 
#- Arguments:
#-  - a2: button currently pressed flag
#- Returns:
#-  - a0: positive edge detected
#- Tweaked registers - t0, t1
#---------------------------------------------------------------- 
debounceStep:
    # t0 = pressed duration
    la t0, pressedDuration
    lb t0, 0(t0)

    bne a2, zero, btnPressed
btnNotPressed:
    li t0, BTN_DURATION         # Reset pressed duration
    li a0, 0                    # No positive edge detected
    ret
btnPressed:
    addi t0, t0, -1
    # Has it been pressed for long enough?
    bne t0, zero, positiveEdge
    ret
positiveEdge:
    li a0, 1                    # Positive edge detected
    li t0, -1
    ret
    

#----------------------------------------------------------------
#- Subroutine for doing the draw step
#- Tweaked registers - t0, t1, t2, t3, t4
#---------------------------------------------------------------- 
draw:
    # t0 = index
    la t0, digitIndex
    lb t0, 0(t0)

    # t1 = index * 4
    slli t1, t0, 2

    # t1 = digit value = (bcd << 4*index) & 0xF
    la t2, bcdData
    lb t2, 0(t2)  # t2 = bcd
    srl t1, t2, t1  # t1 = bcd << 4*index
    andi t1, t1, 0xF 

isLastDigitIndex:
    # Are we rendering the right-most digit?
    beq t0, x0, buildOutput
isZeroDigitVal:
    # Is the digit value zero?
    bne t1, x0, buildOutput
isDigitEncountered:
    # t2 = digit encountered
    la t2, digitEncountered
    lb t2, 0(t2)

    # Has a non-zero digit been encountered?
    beq s2, x0, buildOutput
    
    # Clear display
    li t3, -1
    li t4, -1
    j doWrite7Seg
buildOutput:
    li t2, 1                    # Set digit encountered

    # t3 = anode output = ~(1 << index)
    li t3, 1
    sll t3, t3, t1
    not t3, t3

    # t4 = segment output = sseg[digit value]
    add t4, s7, t4
    lbu t4, 0(t4)
doWrite7Seg:
    li t5, 0x1100C008           # Anodes
    li t6, 0x1100C004           # Segments

    sw t0, 0(t5)                # Clear anodes
    sw s5, 0(t5)                # Write to anodes
    sw s8, 0(t6)                # Write to segs

# Decrementing reached zero? We do this instead of a simple 
# modular add because we need to clear digit encountered.
shouldLoopAround:
    beq t0, x0, doLooparound
doDecrement:
    addi t0, t0, -1
    j cleanup
doLooparound:
    li t0, 3                    # Set digit to 3
    li t2, 0                    # Clear digit encountered
cleanup:
    la t3, digitEncountered
    sb t2, 0(t3)
    la t3, digitIndex
    sb t0, 0(t3)
    ret

#----------------------------------------------------------------
#- Converts a value less than 10000 into its 4-digit BCD 
#- representation. Adapted from Lab 4.
#-
#- Parameters
#-      a0 - value to convert
#- Returns
#-      a1 - BCD representation
#- Tweaked registers - t0, t2, a1
#---------------------------------------------------------------- 
bcd:
    addi sp, sp, -12
    sw ra, 0(sp)
    sw a0, 4(sp)
    sw a2, 8(sp)

    mv a1, a0                   # Dividend
    li a2, 1000                 # Divisor
    call divide                 # a0 R a1 = a1 / a2

    mv t2, a0                   # Quotient = thousands digit.
    slli t2, t2, 4              # Shift BCD

    li a2, 100                  # Divisor. Modulus = new dividend.
    call divide                 # a0 R a1 = a1 / a2

    or t2, t2, a0               # Quotient = hundreds digit.
    slli t2, t2, 4              # Shift BCD.
    
    li a2, 10                   # Divisor. Modulus = new dividend.
    call divide                 # a0 R a1 = a1 / a2

    or t2, t2, a0               # Quotient = ten's digit.
    slli t2, t2, 4              # Shift BCD.
    or t2, t2, a1               # Modulus = one's digit.

    mv a1, t2                   # Return value

    lw ra, 0(sp)
    lw a0, 4(sp)
    lw a2, 8(sp)
    addi sp, sp, 12

    ret

#----------------------------------------------------------------
#- Implementation of binary division. Given a dividend n and 
#- divisor p, runs in approximately log(n / p) time.
#-
#- Parameters
#-      a1 - dividend
#-      a2 - divisor
#- Returns
#-      a0 - quotient
#-      a1 - modulus/remainder
#- Tweaked registers - a0, a1, t0
#----------------------------------------------------------------
divide:
    blt a1, a2, iszero          # If dividend < divisor return early

    # Store registers on stack
    addi sp, sp, -4
    sw a2, 0(sp)

    # Initialize registers
    li t0, 1  # divisor coefficient

# Double the divisor until it is greater than dividend
expand:
    slli t0, t0, 1
    slli a2, a2, 1
    bgt a1, a2, expand

    beq a1, a2, isPower2


beginSubtract: 
    li a0, 0                    # quotient
# Halve the divisor until we go under the original value
subtractloop:  
    srai t0, t0, 1
    beq zero, t0, enddiv        # End
    srai a2, a2, 1
    blt a1, a2, subtractloop    # Don't subtract if a1 < a2

# Add to quotient if divisor falls below modulus/dividend
subtract:
    or a0, a0, t0
    sub a1, a1, a2
    bne a1, x0, subtractloop
enddiv:
    lw a2, 0(sp)
    addi sp, sp, 4
    ret
isPower2:
    mv a0, t0
    li a1, 0
    j enddiv 
iszero:
    li a0, 0
    ret

end: j end
