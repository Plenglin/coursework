.equ LEDS, 0x1100C004
.global main
.type main, @function
.text
main:
init:
li x15,0x1100C004
 # put output address into register
la x6,ISR
 # load address of ISR into x6
csrrw x0,mtvec,x6
 # store address as interrupt vector CSR[mtvec]
mv x8, x0
mv x20, x0
sw x20, 0(x15)

# clear x8; use as flag
# keep track of current output value
# put LEDs in known state
li x10, 1
csrrw x0, mie, x10
# set value in x10
# enable interrupts
loop:
nop
beq x8,x0,loop
# do nothing (easier to see in simulator)
# wait for interrupt
xori x20, x20, 1
sw x20, 0(x15)
# toggle current LED value
# output LED value
mv x8,x0
 # clear flag
csrrw x0,mie,x10
 # enable interrupt
j loop
 # return to loopville
#-----------------------------------------------------------------------
#- The ISR: sets bit x8 to act as flag task code.
#-----------------------------------------------------------------------
ISR:
li x8,1 # set flag to non-zero
mret
 # return from interrupt
#-----------------------------------------------------------------------
