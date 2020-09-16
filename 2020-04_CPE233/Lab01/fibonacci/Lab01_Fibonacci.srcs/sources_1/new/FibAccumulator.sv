`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 05:04:55 PM
// Design Name: 
// Module Name: FibAccumulator
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


module FibAccumulator#(parameter WIDTH=16)(
    input clr,
    input clk,
    input en,
    output reg [WIDTH-1:0] sum = 1
    );
    
    reg [WIDTH-1:0] prev = 0;
    
    always_ff @(posedge clk) begin
        if (clr) begin
            prev <= 0;
            sum <= 1;
        end else if (en) begin
            prev <= sum;
            sum <= prev + sum;
        end
    end
    
endmodule
