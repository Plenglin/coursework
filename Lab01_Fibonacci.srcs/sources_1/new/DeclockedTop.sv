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
// Description: The top level is separated from its clock divider to make it easier 
// to run simulations with. 
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
    
    ClockDivider #(.WIDTH(32)) div(.clkin(clk), .n(32'd25000000));  // Bring the clock down from 100MHz to 1Hz 
    Top top(.clk(div.clkout), .clk_fast(clk), .*);
    
endmodule
