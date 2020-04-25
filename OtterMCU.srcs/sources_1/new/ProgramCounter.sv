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
    input [31:0] jalr,
    input [31:0] jal,
    input [31:0] branch,
    input pc_write,
    input [1:0] pc_source,
    input clk,
    output [13:0] addr
    );
    
    reg [13:0] data;
    assign addr = data;
    logic [13:0] next;
    
    always_comb begin
        case (pc_source) 
            2'd0: next = data + 1;
            2'd1: next = jalr[15:2];
            2'd2: next = branch[15:2];
            default: next = jal[15:2];
        endcase
    end
    
    always_ff @(posedge clk) begin
        if (rst)
            data <= 0;
        else if (pc_write) 
            data <= next;
    end
endmodule
