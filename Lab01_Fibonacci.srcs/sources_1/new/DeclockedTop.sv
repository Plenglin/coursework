`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 04:54:49 PM
// Design Name: 
// Module Name: DeclockedTop
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


module DeclockedTop(
    input btn,
    input clk,
    output [3:0] an,
    output [6:0] seg,
    output [3:0] led
    );
    
    ClockDivider #(.WIDTH(64)) div(.clkin(clk), .n(64'd100000000));  // Bring the clock down from 100MHz to 1Hz 
    Top top(.clk(div.clkout), .*);
    
endmodule
