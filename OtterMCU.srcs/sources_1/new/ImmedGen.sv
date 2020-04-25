`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/24/2020 04:20:00 PM
// Design Name: 
// Module Name: immed_gen
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


module ImmedGen(
    input [31:7] ir,
    output [31:0] b_type_imm,
    output [31:0] j_type_imm,
    output [31:0] i_type_imm,
    output [31:0] s_type_imm,
    output [31:0] u_type_imm
    );
    
    logic isbj_sign;
    assign isbj_sign = ir[31];
    
    assign i_type_imm = {{21{isbj_sign}}, ir[30:25], ir[24:20]};
    assign s_type_imm = {{21{isbj_sign}}, ir[30:25], ir[11:7]};
    assign b_type_imm = {{20{isbj_sign}}, ir[7], ir[30:25], ir[11:8], 1'b0};
    assign u_type_imm = {ir[31:12], 12'b0};
    assign j_type_imm = {{12{isbj_sign}}, ir[19:12], ir[20], ir[30:21], 1'b0};
endmodule
