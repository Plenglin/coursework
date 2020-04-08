`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 04:54:49 PM
// Design Name: 
// Module Name: Top
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


module Top(
    input btn,
    input clk,
    output [3:0] an,
    output [6:0] seg,
    output [3:0] led
    );
    
    FibCalculator calc(.calculate(btn), .clk(clk), .addr(led));
    univ_sseg display(.clk(clk), .cnt1({3'b0, calc.disp_num}), .ssegs(seg), .valid(calc.ready), .disp_en(an));
endmodule
