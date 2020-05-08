`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Ratner Surf Designs
// Engineer: James Ratner
// 
// Create Date: 01/29/2019 04:56:13 PM
// Design Name: 
// Module Name: CU_Decoder
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies:
// 
// CU_DCDR my_cu_dcdr(
//   .br_eq     (), 
//   .br_lt     (), 
//   .br_ltu    (),
//   .opcode    (),    //-  ir[6:0]
//   .func7     (),    //-  ir[31:25]
//   .func3     (),    //-  ir[14:12] 
//   .alu_fun   (),
//   .pcSource  (),
//   .alu_srcA  (),
//   .alu_srcB  (), 
//   .rf_wr_sel ()   );
//
// 
// Revision:
// Revision 1.00 - File Created (02-01-2020) - from Paul, Joseph, & Celina
//          1.01 - (02-08-2020) - removed unneeded else's; fixed assignments
//          1.02 - (02-25-2020) - made all assignments blocking
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module CU_DCDR(
    input br_eq, 
    input br_lt, 
    input br_ltu,
    input [6:0] opcode,   //-  ir[6:0]
    input [6:0] func7,    //-  ir[31:25]
    input [2:0] func3,    //-  ir[14:12] 
    output logic [3:0] alu_fun,
    output logic [1:0] pcSource,
    output logic alu_srcA,
    output logic [1:0] alu_srcB, 
    output logic [1:0] rf_wr_sel
    );
    
    //- datatypes for RISC-V opcode types
    typedef enum logic [6:0] {
        LUI    = 7'b0110111,
        AUIPC  = 7'b0010111,
        JAL    = 7'b1101111,
        JALR   = 7'b1100111,
        BRANCH = 7'b1100011,
        LOAD   = 7'b0000011,
        STORE  = 7'b0100011,
        OP_IMM = 7'b0010011,
        OP_RG3 = 7'b0110011
    } opcode_t;
    opcode_t OPCODE; //- define variable of new opcode type
    
    assign OPCODE = opcode_t'(opcode); //- Cast input enum 

    //- datatype for func3Symbols tied to values
    typedef enum logic [2:0] {
        //BRANCH labels
        BEQ = 3'b000,
        BNE = 3'b001,
        BLT = 3'b100,
        BGE = 3'b101,
        BLTU = 3'b110,
        BGEU = 3'b111
    } func3_t;    
    func3_t FUNC3; //- define variable of new opcode type
    
    assign FUNC3 = func3_t'(func3); //- Cast input enum 
    
    // Branch condition selector
    logic raw_branch_cond;
    always_comb case(func3[2:1])
        2'b00: raw_branch_cond = br_eq;     // BEQ, BNE
        2'b10: raw_branch_cond = br_lt;     // BLT, BGE
        2'b11: raw_branch_cond = br_ltu;    // BLTU, BGEU
        default: raw_branch_cond = 0;       // ruh roh
    endcase
    
    logic branch_cond;
    assign branch_cond = raw_branch_cond ^ func3[0];
    
    // ALU translator
    logic [3:0] alu_op_alu_fun;
    assign alu_op_alu_fun = {func7[5], func3};
       
    always_comb begin 
        //- schedule all values to avoid latch
        pcSource = 2'b00;  
        rf_wr_sel = 2'b00; 
        
        alu_srcA = 1'b0;   
        alu_srcB = 2'b00;    
        alu_fun  = 4'b0000;
        
        case(OPCODE)
            LUI: begin
                alu_fun = 4'b1001;   // lui
                alu_srcA = 1;        // u-imm 
                rf_wr_sel = 2'b11;   // alu_result
            end
            
            AUIPC: begin
                alu_fun = 4'b0000;   // add
                alu_srcA = 1;        // u-imm
                alu_srcB = 4'd3;     // pc
                rf_wr_sel = 2'd3;   // alu_result
            end
            
            JAL: begin
                rf_wr_sel = 2'd0;   // next pc
                pcSource = 2'd3;     // jal
            end
            
            JALR: begin
                rf_wr_sel = 2'd0;   // next pc
                pcSource = 2'd1;       // jalr
            end
            
            LOAD: begin
                alu_fun = 4'b0000;     // add
                alu_srcA = 0;          // rs1
                alu_srcB = 2'd1;       // i imm
                rf_wr_sel = 2'd2;      // mem dout
            end
            
            STORE: begin
                alu_fun = 4'b0000;     // add
                alu_srcA = 1'b0;       // rs1
                alu_srcB = 2'd2;       // s imm
            end
            
            BRANCH: 
                pcSource = branch_cond   // Invert if invert bit 
                    ? 2'd2      // Condition success, branch
                    : 2'd0;     // Condition fail, next
            
            OP_IMM: begin
                pcSource = 2'b00;  // next
                alu_srcA = 1'b0;   // rs1
                alu_srcB = 2'd1;   // i imm
                rf_wr_sel = 2'd3;  // alu result
                alu_fun = alu_op_alu_fun;  // translated func
            end
            
            OP_RG3: begin
                pcSource = 2'b00;  // next
                alu_srcA = 0;   // rs1
                alu_srcB = 2'd0;   // rs2
                rf_wr_sel = 2'd3;  // alu result
                alu_fun = alu_op_alu_fun;  // translated func             
            end

            default: begin
                 pcSource = 2'b00; 
                 alu_srcB = 2'b00; 
                 rf_wr_sel = 2'b00; 
                 alu_srcA = 1'b0; 
                 alu_fun = 4'b0000;
            end
        endcase
    end

endmodule