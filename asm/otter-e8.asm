.data
sseg: .space 10 # Save space for 10-byte lookup table
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
    
    li sp, 0x4000

    li a0, 980                  # Pressed times count
    li s1, 1                    # Constant 1
    li s2, 0                    # Digit encountered flag

    li s3, 1                    # Pressed flag (for lazy calculation)
    li a1, 0x03                 # Cached BCD representation
    
    li s6, 3                    # Digit index
    li s9, 0x1100C008           # Anodes
    li s10, 0x1100C004          # Segments

    # Enable ISR
    la t0, isr
    csrrw x0, mtvec, t0 
    csrrw x0, mie, s1 

doMainLoop:
    li s8, -1                   # Default seg output
    li s5, -1                   # Default anode output
isPressed:
    beq s3, x0, extractDigit    # Did the button get pressed?
doCalcBCD:
    call bcd                    # Calculate new BCD
    li s3, 0                    # Clear pressed flag
    csrrw x0, mie, s1           # Raise MIE 
extractDigit:
    # s4 = 4 * index
    slli s4, s6, 2

    # s4 = digit value = (bcd << 4*index) & 0xF
    srl s4, a1, s4
    andi s4, s4, 0xF 

isLastDigitIndex:
    # Are we rendering the right-most digit?
    beq s6, x0, buildOutput
isZeroDigitVal:
    # Is the digit value zero?
    bne s4, x0, buildOutput
isDigitEncountered:
    # Has a non-zero digit been encountered?
    beq s2, x0, doWrite7Seg
buildOutput:
    li s2, 1                    # Set digit encountered

    # s5 = ~(1 << (digit index))
    sll s5, s1, s6
    not s5, s5

    # s8 = segment output (sseg[digit value])
    add s8, s7, s4
    lbu s8, 0(s8)
doWrite7Seg:
    li t0, -1
    sw t0, 0(s9)                # Write to anodes
    sw s8, 0(s10)               # Write to segs
    sw s5, 0(s9)                # Write to anodes

# Decrementing reached zero? We do this instead of a simple 
# modular add because we need to clear digit encountered.
shouldLoopAround:
    beq s6, x0, doLooparound
doDecrement:
    addi s6, s6, -1
    j delay
doLooparound:
    li s6, 3                    # Set digit to 3
    li s2, 0                    # Clear digit encountered
delay:
    li t0, 200
delayLoop:
    addi t0, t0, -1
    bgt t0, x0, delayLoop

    j doMainLoop

isr:
    addi a0, a0, 1              # Increment pressed times
    li s3, 1                    # Set the pressed flag
    mret

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
