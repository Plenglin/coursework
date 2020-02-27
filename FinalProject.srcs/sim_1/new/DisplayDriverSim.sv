`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/25/2020 02:53:29 PM
// Design Name: 
// Module Name: DisplayDriverSim
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


module DisplayDriverSim(

    );
    
    logic [7:0] rule;
    logic clk, digit;
    logic [7:0] seg;
    logic [3:0] an;

    
    DisplayDriver drv(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #1;
    end
    
    initial begin
        rule = 8'h05;
        digit = 0;
        #20;
        digit = 1;
        #20;
        rule = 8'ha5;
        digit = 0;
        #20;
        rule = 8'ha0;
    end
endmodule
