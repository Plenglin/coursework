`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 06:26:00 PM
// Design Name: 
// Module Name: FibWriter
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


module FibWriter #(parameter DATA_WIDTH=16, ADDR_WIDTH=4) (
    input en,
    input clk,
    input [ADDR_WIDTH-1:0] count,
    output [DATA_WIDTH-1:0] data,
    output [ADDR_WIDTH-1:0] addr,
    output write
    );
    
    logic reached_end;
    assign reached_end = addr >= count;
    
    FibWriterCtrl #(.ADDR_WIDTH(ADDR_WIDTH)) ctrl(.clk(clk), .en(en), .addr(addr), .last_addr(count-1), .write(write));
    FibAccumulator #(.WIDTH(DATA_WIDTH)) acc(.clk(clk), .en(write), .clr(~write), .sum(data));
    
endmodule
