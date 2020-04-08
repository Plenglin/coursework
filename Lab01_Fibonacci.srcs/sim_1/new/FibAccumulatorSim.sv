`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 05:09:03 PM
// Design Name: 
// Module Name: FibAccumulatorSim
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


module FibAccumulatorSim(

    );
    
    logic clk, clr, en;
    logic [7:0] sum;
    
    FibAccumulator #(.WIDTH(8)) facc(.*);
    
    always begin
        clk = 0;
        #1;
        clk = 1;
        #1;
    end
    
    initial begin
        clr = 0;
        en = 0;
        #10;
        
        en = 1;
        #20;
        
        en = 0;
        #10;
        
        clr = 0;
        #10;
    end;
    
endmodule
