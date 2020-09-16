#----------------------------------------------------------------
#- Firmware debounce loop.
#---------------------------------------------------------------- 

.text
.eqv DURATION, 8            # How many cycles before debounced?
initialize:
    li s0, 0x11008004       # Button port

resetCount:
    # Do while NOT button pressed things
    li s1, 8                # Count
loadButtonState:
    lw t0, 0(s0)            # Button state
    andi t0, t0, 1
isButtonPressed:
    beq t0, zero, resetCount
isButtonFullyDebounced:
    bgt s1, zero, whileButtonPressed
decrementCount:
    addi s1, s1, -1
    j loadButtonState

whileButtonPressed:
    # do stuff
    j loadButtonState

