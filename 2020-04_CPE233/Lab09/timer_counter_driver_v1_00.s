
#--------------------------------------------------------------------
#- Engineer: James Ratner
#- Company: Barefoot Engineering
#- 
#- The program tests a background timer counter modules that can be 
#- used to provide background t(hardware) timing for RISC-V applications. 
#- This program blinks the LED(7) as a sanity check, the 
#- the first four switches are used to turn on the right-four LEDs. 
#---------------------------------------------------------------------
.text

     
init:    la     x6,ISR          # load address of ISR into x6
         csrrw  x0,mtvec,x6        # store address as interrupt vector CSR[mtvec]
         csrw   x6,mtvec
         
         li     x10,0x1100C000  # LED port address
         li     x11,0x11008000  # Switch port address
         li     x12,0x1100D000  # timer counter CSR port address
         li     x13,0x1100D004  # timer counter count port address
         
         mv     x31,x0          # clear LED register
                  
         li     x20,0x00FFFFFF  # for ~1.5Hz blink rate
         sw     x20,0(x13)      # init TC count
         
         li     x20,0x01        # init TC CSR
         sw     x20,0(x12)      # no prescale, turn on TC
         
         li     x9,1            # store a 1
         mv     x8,x9           
         csrrw  x0,mie,x8       # enable interrupts   
            
main:    lw     x20,0(x11)      # get switch value
         andi   x20,x20,0xF     # mask low nibble
         
         or     x20,x20,x31     # include bit 7 
         sw     x20,0(x10)      # output to LEDS

         csrrw  x0,mie,x9       # enable interrupt
         j      main            # rinse, repeat         
#----------------------------------------------------------------------
#- ISR: toggles bit(7) of r31
#----------------------------------------------------------------------
ISR:     xori   x31,x31,0x80   #  toggle LED output (MSB LED)
         mret
#----------------------------------------------------------------------
