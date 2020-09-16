.text

# This section exists to test the subroutine.
main:
    li x10, 1234
    call bcd

    li x10, 10
    call bcd

    li x10, 0
    call bcd

    li x10, 3483
    call bcd

    li x10, 13
    call bcd

    li x10, 69
    call bcd

    li x10, 832
    call bcd
    j end

#----------------------------------------------------------------
#- Converts a value less than 10000 into its 4-digit BCD 
#- representation.
#-
#- Parameters
#-      a0/x10 - value to convert
#- Returns
#-      a1/x11 - BCD representation
#- Tweaked registers - x11
#---------------------------------------------------------------- 
bcd:
    # t0 is the register used for accumulating the BCD.
    addi sp, sp, -16
    sw t0, 0(sp)
    sw a0, 4(sp)
    sw a2, 8(sp)
    sw ra, 12(sp)

    mv a1, a0       # Dividend
    li a2, 1000     # Divisor
    call divide     # a0 R a1 = a1 / a2

    mv t0, a0       # Quotient = thousands digit.
    slli t0, t0, 4  # Shift BCD

    li a2, 100      # Divisor. Modulus = new dividend.
    call divide     # a0 R a1 = a1 / a2

    or t0, t0, a0   # Quotient = hundreds digit.
    slli t0, t0, 4  # Shift BCD.
    
    li a2, 10       # Divisor. Modulus = new dividend.
    call divide     # a0 R a1 = a1 / a2

    or t0, t0, a0   # Quotient = ten's digit.
    slli t0, t0, 4  # Shift BCD.
    or t0, t0, a1   # Modulus = one's digit.

    mv a1, t0       # Return value

    lw t0, 0(sp)
    lw a0, 4(sp)
    lw a2, 8(sp)
    lw ra, 12(sp)
    addi sp, sp, 16

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
#- Tweaked registers - a0, a1
#----------------------------------------------------------------
divide:
    blt a1, a2, iszero  # If dividend < divisor return early

    # Store registers on stack
    addi sp, sp, -8
    sw t0, 0(sp)
    sw a2, 4(sp)

    # Initialize registers
    li t0, 1  # divisor coefficient

expand:  # Double the divisor until it is greater than dividend
    slli t0, t0, 1
    slli a2, a2, 1
    bgt a1, a2, expand

    li a0, 0  # quotient
subtractloop:  # Halve the divisor until we go under the original value
    srai t0, t0, 1
    bne zero, t0, subtractloop
    srai a2, a2, 1
    blt a1, a2, nosub
subtract:
    # Add to quotient if divisor falls below modulus/dividend
    or a0, a0, t0
    sub a1, a1, a2
nosub:
    j subtractloop

    lw t0, 0(sp)
    lw a2, 4(sp)
    addi sp, sp, 8
    ret
iszero:
    li a0, 0
    ret

end: 
