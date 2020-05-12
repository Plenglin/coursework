`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Cal Poly
// Engineer: Astrid Yu
// 
// Create Date: 04/29/2020 02:27:42 PM
// Design Name: Main Otter MCU Module
// Module Name: OTTER_MCU
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


module OTTER_MCU #(
    parameter MEM_FILE="otter_memory.mem"
    ) (
    input RST,
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
        pc_next,
        ir, 
        reg_wd,
        rs1,
        rs2,

        alu_src_a_data,
        alu_src_b_data,
        alu_result, 
        
        jalr, 
        branch, 
        jal, 
        
        mem_dout,
        
        b_type_imm, 
        i_type_imm, 
        j_type_imm,
        s_type_imm,
        u_type_imm,
        
        mepc,
        mtvec,
        csr_reg;
    
    // Selectors
    logic [1:0] rf_wr_sel, alu_src_b;
    logic [2:0] pc_source;

    logic alu_src_a;
    logic [3:0] alu_fun;
    
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
        br_ltu,
        
        int_taken,
        csr_we,
        csr_mie;
    
    assign iobus_addr = alu_result;
    assign iobus_out = rs2;
    
    // Multiplexers    
    always_comb case(rf_wr_sel)
        4'd0: reg_wd = pc + 4;
        4'd1: reg_wd = csr_reg;
        4'd2: reg_wd = mem_dout;
        4'd3: reg_wd = alu_result;
    endcase
        
    assign alu_src_a_data = alu_src_a 
        ? u_type_imm 
        : rs1;

    always_comb case(alu_src_b)
        4'd0: alu_src_b_data = rs2;
        4'd1: alu_src_b_data = i_type_imm;
        4'd2: alu_src_b_data = s_type_imm;
        4'd3: alu_src_b_data = pc;
    endcase
    
    always_comb case(pc_source) 
        3'd0: pc_next = pc_inc;
        3'd1: pc_next = jalr;
        3'd2: pc_next = branch;
        3'd3: pc_next = jal;
        3'd4: pc_next = mtvec;
        3'd5: pc_next = mepc;
    endcase
    
    CSR csr(
        .CLK(clk),
        .RST(reset),
        
        .ADDR(ir[31:20]),

        .INT_TAKEN(int_taken),
        .WR_EN(csr_we),
        .PC(pc),
        .WD(rs1),
        
        .CSR_MIE(csr_mie),
        .CSR_MEPC(mepc),
        .CSR_MTVEC(mtvec),
        .RD(csr_reg)
    );
        
    // Submodules
    CU_FSM fsm(
        .clk(clk),
        
        .RST(RST), 
        .intr(intr),
        .opcode(ir[6:0]),
        
        .pcWrite(pc_write),
        .regWrite(reg_write),
        .memWE2(mem_we2),
        .memRDEN1(mem_rden1),
        .memRDEN2(mem_rden2),
        .reset(reset)
    );
    
    BranchCondGen bcg(
        .rs1(rs1),
        .rs2(rs2),
        .br_eq(br_eq),
        .br_lt(br_lt),
        .br_ltu(br_ltu)
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

        .pc_write(pc_write),
        .rst(reset),
        .next(pc_next),
        
        .addr(pc),
        .addr_inc(pc_inc)
    );

    Memory #(.MEM_FILE(MEM_FILE)) mem(
        .MEM_CLK (clk),
        
        .MEM_RDEN1 (mem_rden1),
        .MEM_RDEN2 (mem_rden2),
        .MEM_WE2 (mem_we2),
        .MEM_ADDR1 (pc[15:2]),
        .MEM_ADDR2 (alu_result),
        .MEM_DIN2 (rs2),
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
        .srcA(alu_src_a_data),
        .srcB(alu_src_b_data),
        
        .result(alu_result)
    );
    
endmodule
