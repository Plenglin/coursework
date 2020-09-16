`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/17/2020 04:58:54 PM
// Design Name: 
// Module Name: Exp7Wrapper
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module Exp7Wrapper(
    input CLK,
    input BTNL,
    input BTNC,
    input [15:0] SWITCHES,
    output logic [15:0] LEDS,
    output [7:0] CATHODES,
    output [3:0] ANODES
    );
    
    logic sclk, reset, count, interrupt;
    assign reset = BTNL;
    assign count = BTNC;
    
    one_shot_bdir  #(.n(3)) my_oneshot (
        .clk(sclk),
        .sig_in(),
        .pos_pulse_out(count), 
        .neg_pulse_out()  
    ); 
    DBounce #(.n(5)) my_dbounce(
        .clk    (sclk),
        .sig_in (),
        .DB_out ()   
    );
    
           
        clk_div clkDIv(CLK, sclk);
    OtterMCU mcu(
         .RST         (reset),
         .intr        (interrupt),
         .clk         (sclk),
         .iobus_in    (IOBUS_in),
         .iobus_out   (IOBUS_out), 
         .iobus_addr  (IOBUS_addr), 
         .iobus_wr    (IOBUS_wr)   
     );
endmodule
