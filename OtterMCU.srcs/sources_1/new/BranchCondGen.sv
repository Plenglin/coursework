`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/04/2020 02:43:37 PM
// Design Name: 
// Module Name: BranchCondGen
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


module BranchCondGen(
    input [31:0] rs1,
    input [31:0] rs2,
    output br_eq,
    output br_lt,
    output br_ltu
    );
    
    assign br_eq = rs1 == rs2;
    assign br_lt = $signed(rs1) < $signed(rs2);
    assign br_ltu = rs1 < rs2;
endmodule
