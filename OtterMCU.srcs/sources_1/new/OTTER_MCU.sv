`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/29/2020 02:27:42 PM
// Design Name: 
// Module Name: OTTER_MCU
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


module OTTER_MCU(
    input rst,
    input clk,
    input [31:0] iobus_in,
    input intr,
    output iobus_wr,
    output [31:0] iobus_out,
    output [31:0] iobus_addr
    );
    
    // Data buses
    logic [31:0] 
        pc, 
        pc_inc,
        ir, 
        reg_wd,
        rs1,
        rs2,

        alu_src_b_data,
        alu_result, 
        
        jalr, 
        branch, 
        jal, 
        
        mem_addr, 
        mem_dout,
        
        b_type_imm, 
        i_type_imm, 
        j_type_imm,
        s_type_imm,
        u_type_imm;
    
    // Selectors
    logic [1:0] pc_source, rf_wr_sel, alu_src_b;
    logic [3:0] alu_fun;
    logic alu_src_a;
    
    // Flags
    logic 
        reset,
        pc_write,
        
        reg_write,
        
        mem_we2,
        mem_rden1,
        mem_rden2,
        
        br_eq,
        br_lt,
        br_ltu;
    
    assign iobus_addr = alu_result;
    assign iobus_out = rs2;
    
    // Multiplexers    
    always_comb case(rf_wr_sel)
        4'd0: reg_wd = pc_inc;
        4'd2: reg_wd = mem_dout;
        4'd3: reg_wd = mem_addr;
    endcase
    
    always_comb case(alu_src_b)
        4'd0: alu_src_b_data = rs2;
        4'd1: alu_src_b_data = i_type_imm;
        4'd2: alu_src_b_data = s_type_imm;
        4'd3: alu_src_b_data = pc;
    endcase
    
    // Submodules
    CU_FSM fsm(
        .clk(clk),
        
        .RST(rst), 
        .intr(intr),
        .opcode(ir[6:0]),
        
        .pcWrite(pc_write),
        .regWrite(reg_write),
        .memWE2(mem_we2),
        .memRDEN1(mem_rden1),
        .memRDEN2(mem_rden2),
        .reset(reset)
    );
    
    CU_DCDR cu_dcdr(
        .opcode(ir[6:0]),
        .func7(ir[31:25]),
        .func3(ir[14:12]),
        
        .br_eq(br_eq),
        .br_lt(br_lt),
        .br_ltu(br_ltu),
        
        .alu_fun(alu_fun),
        .pcSource(pc_source),
        .alu_srcA(alu_src_a),
        .alu_srcB(alu_src_b), 
        .rf_wr_sel(rf_wr_sel)
    );
    
    ProgramCounter prog_counter(
        .clk(clk),

        .rst(reset),
        .pc_source(pc_source),
        .pc_write(pc_write),
        .jal(jal),
        .jalr(jalr),
        .branch(branch),
        
        .addr(pc),
        .addr_inc(pc_inc)
    );

    Memory mem(
        .MEM_CLK (clk),
        
        .MEM_RDEN1 (mem_rden1),
        .MEM_RDEN2 (mem_rden2),
        .MEM_WE2 (mem_we2),
        .MEM_ADDR1 (pc),
        .MEM_ADDR2 (mem_addr),
        .MEM_DIN2 (0),
        .MEM_SIZE (ir[13:12]),
        .MEM_SIGN (ir[14]),
        .IO_IN (iobus_in),
        .IO_WR (iobus_wr),
        
        .MEM_DOUT1 (ir),
        .MEM_DOUT2 (mem_dout) 
    );
    
    RegFile regfile(
        .clk(clk),
        
        .en(reg_write),
        .wd(reg_wd),
        .adr1(ir[19:15]),
        .adr2(ir[24:20]),
        .wa(ir[11:7]),
        
        .rs1(rs1),
        .rs2(rs2)
    );
    ImmedGen imd(
        .ir(ir[31:7]),
        
        .b_type_imm(b_type_imm),
        .i_type_imm(i_type_imm),
        .u_type_imm(u_type_imm),
        .j_type_imm(j_type_imm),
        .s_type_imm(s_type_imm)
    );
    
    BranchAddrGen bag(
        .rs(rs1),
        .pc(pc),
        .b_type_imm(b_type_imm),
        .j_type_imm(j_type_imm),
        .i_type_imm(i_type_imm),
        
        .jalr(jalr),
        .branch(branch),
        .jal(jal)
    );
    
    ALU alu(
        .alu_fun(alu_fun),
        .srcA(alu_src_a ? u_type_imm : rs1),
        .srcB(alu_src_b_data),
        
        .result(alu_result)
    );
    
endmodule
