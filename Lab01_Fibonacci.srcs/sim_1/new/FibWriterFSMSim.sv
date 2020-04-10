`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/09/2020 08:20:38 PM
// Design Name: 
// Module Name: FibWriterFSMSim
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


module FibWriterFSMSim(

    );
    
    logic en;
    logic fin;
    logic clk;
    logic write;
    logic reset;
    
    FibWriterFSM fsm(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #1;
    end
    
    initial begin
        en = 0;
        fin = 0;
        #20;
        
        en = 1;
        #6;
        en = 0;
        #20;
        
        fin = 1;
        #2;
        fin = 0;
        #50;
        
        en = 1;
        #4;
        en = 0;
        #2
        en = 1;
        #4;
        en = 0;
        #20;
        
        fin = 1;
        #2;
        fin = 0;
        #10;

    end

endmodule
