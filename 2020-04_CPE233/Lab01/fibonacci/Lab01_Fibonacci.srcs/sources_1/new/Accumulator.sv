`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 10:01:01 PM
// Design Name: 
// Module Name: Accumulator
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


module Accumulator #(parameter WIDTH=4)(
    input ld,
    input clr,
    input clk,
    input [WIDTH-1:0] data,
    output reg [WIDTH-1:0] sum = 0
    );
    
    always_ff @(posedge clk) begin
        if (clr) 
            sum <= 0;
        else if (ld) 
            sum <= sum + data;
    end
    
endmodule
