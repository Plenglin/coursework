`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 10:05:41 PM
// Design Name: 
// Module Name: RAMReaderSim
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


module RAMReaderSim(

    );
    
    logic en;
    logic clr;
    logic clk;
    logic [3:0] addr;
    
    RAMReader rr(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #1;
    end
    
    initial begin
        en = 0;
        clr = 0;
        clk = 0;
        #10;
        
        en = 1;
        #50;
        
        en = 0;
        clr = 1;
        #20;
        
        en = 1;
        clr = 0;
    end

endmodule
