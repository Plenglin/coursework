`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/08/2020 02:42:54 PM
// Design Name: 
// Module Name: ClockAdjOtterWrapper
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


module ClockAdjOtterWrapper(
    input clk,
    input [4:0] buttons,
    input [15:0] switches,
    output logic [15:0] leds,
    output logic [7:0] segs,
    output logic [3:0] an
    );
    
    logic sclk;
    clk_div div(.clk(clk), .sclk(sclk));
    OTTER_Wrapper otter(.clk(sclk), .*);
endmodule
