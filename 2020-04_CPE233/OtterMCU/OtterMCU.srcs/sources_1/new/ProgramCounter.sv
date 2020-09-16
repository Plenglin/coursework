`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Cal Poly
// Engineer: Astrid Yu
// 
// Create Date: 04/13/2020 02:05:38 PM
// Design Name: Program Counter Module
// Module Name: pc_mod
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: Program Counter
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ProgramCounter(
    input rst,
    input [31:0] next,
    input pc_write,
    input clk,
    output [31:0] addr,
    output [31:0] addr_inc
    );
    
    reg [13:0] data;
    logic [13:0] data_inc;
    assign data_inc = data + 1;

    assign addr = {16'b0, data, 2'b0};
    assign addr_inc = {16'b0, data_inc, 2'b0};
    
    always_ff @(posedge clk) begin
        if (rst)
            data <= 0;
        else if (pc_write) 
            data <= next[15:2];
    end
endmodule
