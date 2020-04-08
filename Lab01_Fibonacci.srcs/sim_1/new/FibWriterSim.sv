`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 09:16:50 PM
// Design Name: 
// Module Name: FibWriterSim
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


module FibWriterSim(

    );
    
    logic en;
    logic clk;
    logic [3:0] count = 4'd13;
    logic [7:0] data;
    logic [3:0] addr;
    logic write;

    FibWriter fwr(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #1;
    end
    
    initial begin
        en = 0;
        #10;
        
        en = 1;
        #2;
        en = 0;
        #50;
        
        en = 1;
        #8;
        en = 0;
    end
    
endmodule
