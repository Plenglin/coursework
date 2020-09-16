`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 09:47:02 PM
// Design Name: 
// Module Name: FibUnit
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


module FibCalculator(
    input calculate,
    input clk,
    output ready,
    output [3:0] addr,
    output [9:0] disp_num
    );
    
    Memory #(.DATA_WIDTH(10), .ADDR_WIDTH(4)) mem(.clk(clk), .addr_read(addr), .read(disp_num));
    FibWriter #(.DATA_WIDTH(10), .ADDR_WIDTH(4)) writer(.clk(clk), .en(calculate), .ready(ready), .last_addr(4'd15), .write(mem.en_write), .addr(mem.addr_write), .data(mem.data));
    Accumulator acc(.clk(clk), .ld(ready), .clr(~ready), .data(4'b1), .sum(addr));
endmodule
