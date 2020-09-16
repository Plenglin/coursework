`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Ratner Surf Designs
// Engineer:  James Ratner
// 
// Create Date: 01/07/2020 12:59:51 PM
// Design Name: 
// Module Name: tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: Testbench file for Exp 5
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module otter_tb(); 

    logic RST; 
    logic intr; 
    logic clk; 
    logic [31:0] iobus_in; 
    logic [31:0] iobus_addr; 
    logic [31:0] iobus_out; 
    logic iobus_wr;

    OTTER_MCU #(.MEM_FILE("exp7.mem")) my_otter(
        .RST         (RST),
        .intr        (intr),
        .clk         (clk),
        .iobus_in    (iobus_in),
        .iobus_out   (iobus_out), 
        .iobus_addr  (iobus_addr), 
        .iobus_wr    (iobus_wr)   
    );
    
    initial begin
        clk = 0;
        RST = 1;
        intr = 0;
        iobus_in = 32'hdeadbeef;
    end
     
    //- Generate periodic clock signal    
    always begin       
         clk = ~clk;
         #1;
    end                        
         
    initial begin
        #4
        RST = 0;
        #100
        
        intr = 1;
        #6;
        intr = 0;
        #100
        
        intr = 1;
        #6
        intr = 0;
        #4
        
        intr = 1;
        #8;
        intr = 0; 
        
    end
endmodule
