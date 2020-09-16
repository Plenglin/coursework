.eqv PORT_LEDS, 0x1100C000
.eqv PORT_BTN, 0x11008004
.eqv PORT_SEG, 0x1100C004
.eqv PORT_AN, 0x1100C008
.eqv PORT_SW, 0x11008000
.eqv PORT_TC_CNT, 0x1100D004
.eqv PORT_TC_CSR, 0x1100D000

.text
main:
    li s1, 0    # Value last interrupt
    li s2, 0    # Value 2 interrupt ago
    li s3, 0    # Value 3 interrupts ago
    li s4, 0    # Interrupted flag

    # Initialize LEDs
    la t0, PORT_LEDS
    sb zero, 0(t0)

    # Set ISR address
    la t0, ISR
    csrrw zero, mtvec, t0

enableInterrupts:
    # Enable interrupts
    li t0, 1
    csrrw zero, mie, t0

loop:
    beq s4, zero, loop
onInterrupted:
    li s4, 0        # Clear interrupted flag

    # Shift LED history
    mv s3, s2
    mv s2, s1

    # s1 = latest switch value
    li t0, PORT_SW
    lbu s1, 0(t0)

# Perform a linear search for max through 3 elements

    # Arbitrarily choose max value
    mv t3, s1

    # If s2 > s0 then s0 = s2
    bgt s0, s2, maxNotS2
    mv s0, s2
maxNotS2:
    # If s3 > s0 then s0 = s2
    bgt s0, s3, maxNotS3
    mv s0, s3
maxNotS3:
    # Write LEDs
    la t0, PORT_LEDS
    sb t3, 0(t0)
    
    j enableInterrupts

ISR:
    li s4, 1    # Raise interrupted flag
    mret

