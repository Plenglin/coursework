`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Cal Poly
// Engineer: Astrid Yu
// 
// Create Date: 04/19/2020 08:07:54 PM
// Design Name: Otter MCU ALU
// Module Name: alu
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: Performs arithmetic operations for the Otter MCU.
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ALU(
    input [31:0] srcA,
    input [31:0] srcB,
    input [3:0] alu_fun,
    output logic [31:0] result
    );
    
    logic [4:0] shiftB;
    assign shiftB = srcB[4:0];
        
    always_comb begin
        case (alu_fun)
            4'b0000: result = srcA + srcB;                          // add
            4'b1000: result = srcA - srcB;                          // sub
            4'b0110: result = srcA | srcB;                          // or
            4'b0111: result = srcA & srcB;                          // and
            4'b0100: result = srcA ^ srcB;                          // xor 
            4'b0101: result = srcA >> shiftB;                       // srl
            4'b0001: result = srcA << shiftB;                       // sll
            4'b1101: result = $signed(srcA) >>> shiftB;             // sra
            4'b0010: result = $signed(srcA) < $signed(srcB);        // slt
            4'b0011: result = srcA < srcB;                          // sltu
            4'b1001: result = srcA;                                 // lui
            default: result = 32'hDEADBEEF;
        endcase
    end
endmodule
