.eqv PORT_SW 0x11008000
.eqv PORT_BTN 0x11008004
.eqv PORT_LED 0x1100C000 
.eqv RIGHT_BTN_MASK 0x1

.text
main:
    li s0, 0        # Interrupt counter
    li s1, 0        # Interrupted flag

    # Set up interrupts
    la t0, isr
    csrw mtvec, t0 
    li t0, 1
    csrw mie, t0

mainLoop:
    # Have we received an interrupt?
    beq s1, zero, mainLoop
    li s1, 0

    # Read switches
    li t1, PORT_SW
    lw t0, 0(t1)
    
    # Write LEDs
    li t1, PORT_LED
    sw t0, 0(t1)

    # Number of interrupts == 10?
    li t0, 10
    beq s0, t0, int10Loop
    
    # Unmask
    li t0, 1
    csrw mie, t0
    j mainLoop

int10Loop:
    # t1 = button data
    li t0, PORT_BTN
    lw t1, 0(t0)

    # t0 = right button
    andi t0, t1, RIGHT_BTN_MASK

    # restart if right button not pressed
    beq t0, zero, int10Loop

    # t1 = all other buttons
    # always clears the right button bit using xor magic
    xor t1, t1, t0 
    
    # restart if other buttons are pressed
    bne t1, zero, int10loop

    # return to normal functionality
    li t0, 1
    csrw mie, t0
    j mainLoop

isr:
    addi s0, s0, 1
    li s1, 1
    mret
