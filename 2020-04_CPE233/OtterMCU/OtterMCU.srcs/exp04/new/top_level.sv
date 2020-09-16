`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/24/2020 05:42:22 PM
// Design Name: 
// Module Name: top_level
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


module top_level(
    input PCWrite,
    input [1:0] pcSource,
    input clk,
    input rst,
    input [31:0] u_type_imm,
    input [31:0] s_type_imm
    );
    
    logic [31:0] ir, b_type_imm, i_type_imm, j_type_imm, pc_data, jal, branch, jalr;
    
    ProgramCounter prog_ctr(
        .pc_write(PCWrite),
        .pc_source(pcSource),
        .rst(rst),
        .addr(),
        .*
    );
    
    assign pc_data = {16'b0, prog_ctr.addr, 2'b0};
    
    Memory OTTER_MEMORY (
        .MEM_CLK (clk),
        .MEM_RDEN1 (1),
        .MEM_RDEN2 (0),
        .MEM_WE2 (0),
        .MEM_ADDR1 (pc_data[15:2] + 1),
        .MEM_ADDR2 (0),
        .MEM_DIN2 (0),
        .MEM_SIZE (2),
        .MEM_SIGN (0),
        .IO_IN (0),
        .IO_WR (),
        .MEM_DOUT1 (ir),
        .MEM_DOUT2 () 
    );

    
    ImmedGen imd(
        .ir(ir[31:7]),
        .*    
    );
    
    BranchAddrGen bag(
        .rs(32'd12),
        .pc(pc_data),
        .*
    );
endmodule
