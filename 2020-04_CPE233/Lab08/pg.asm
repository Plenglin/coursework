#----------------------------------------------------------------
#- Write an interrupt driven RISC-V MCU assembly language 
#- program that does the following. The program outputs the most 
#- recent value on the switches to the LEDs when it receives an 
#- interrupt. If the program outputs the same switch value on two 
#- consecutive interrupts, it stops outputting values until 
#- BUTTON(0) (the LSB on the BUTTON input) is pressed (button 
#- press = 1), at which point it returns to normal processing.
#-
#- This code uses lots of cool and fancy masking so there's no 
#- branches needed. 
#---------------------------------------------------------------- 
.text

.EQU    LEDS,     0x1100_C000
.EQU    SWITCHES, 0x1100_8000
.EQU    BUTTONS,  0x1100_8004

initialize:
    li s0, LEDS 
    li s1, SWITCHES
    li s2, BUTTONS

    li s3, 0                # Current switches
    li s4, 0                # Last interrupt switch state
    li s5, -1               # Persistent switch mask

    la t0, ISR
    csrrw x0, mtvec, t0 
normalLoop:
    la t1, 1
    csrrw x0, mie, t1       # Raise MIE

    lw s6, 0(s2)            # Read buttons
    andi t0, s6, 1          # Take LSB of buttons
    neg t0, t0              # Convert bit to mask
    or t0, s5, t0           # button state overrides last state

    lw s3, 0(s1)            # Read switches
    and t1, s3, t0          # Mask switches
    sw t1, 0(s0)            # Write masked switches to LEDs

    mv s5, t0               # Store new mask into persistent
    j normalLoop

ISR:
    seq s5, s4, s3          # s5 = (prev intr == current)
    neg s5, s5              # Convert bit to mask
    mv s4, s3               # prev intr <- current
    mret