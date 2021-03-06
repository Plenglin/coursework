`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/04/2021 08:26:34 PM
// Design Name: 
// Module Name: ALU
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - FilWe Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ALU(
    input [31:0] a,
    input [31:0] b,
    input [2:0] aluop,
    output logic carry_out,
    output logic [31:0] result
    );
    
    always_comb begin
        case (aluop) 
            4'b000: result = a & b;
            4'b001: result = a | b;
            4'b010: result = a + b;
            4'b110: result = a - b;
            4'b111: result = a < b ? 32'b1 : 32'b0;
            default: result = 32'hDEADBEEF;  // for debugging
        endcase 
    end
    
    assign carry_out = a[31] & b[31];
    
endmodule
