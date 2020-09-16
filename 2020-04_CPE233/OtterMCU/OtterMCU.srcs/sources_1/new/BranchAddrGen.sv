`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Cal Poly
// Engineer: Astrid Yu
// 
// Create Date: 04/24/2020 04:20:00 PM
// Design Name: Branch Address Generator
// Module Name: branch_addr_gen
// Project Name: Otter MCU
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


module BranchAddrGen(
    input [31:0] pc,
    input [31:0] rs,
    input [31:0] b_type_imm,
    input [31:0] i_type_imm,
    input [31:0] j_type_imm,
    output [31:0] jal,
    output [31:0] branch,
    output [31:0] jalr
    );
    
    assign jal = pc + j_type_imm;
    assign branch = pc + b_type_imm;
    assign jalr = rs + i_type_imm;
    
endmodule
