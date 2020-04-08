`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 08:45:09 PM
// Design Name: 
// Module Name: FibWriterCtrlSim
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


module FibWriterCtrlSim(

    );
    
    logic en;
    logic clk;
    logic [3:0] last_addr = 4'd10;
    logic [3:0] addr;
    logic write;

    FibWriterCtrl fwrc(.*);
    
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
        #6;
        en = 0;
        #6;
        
        en = 1;
        #2;
        en = 0;
    end
endmodule
