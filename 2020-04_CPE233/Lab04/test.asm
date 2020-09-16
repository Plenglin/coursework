.text
        nop
cat:    nop
dog:    nop
        jal     x0,dog          # j
        jalr    x0,-8,x20       # i
        beq     x10,x10,cat     # b
        sw      x10,12(x20)     # s
        lui     x0,255          # u
end:    j end
