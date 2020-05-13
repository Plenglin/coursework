`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:  Ratner Surf Designs
// Engineer: James Ratner
// 
// Create Date: 01/07/2020 09:12:54 PM
// Design Name: 
// Module Name: top_level
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: Control Unit Template/Starter File for RISC-V OTTER
//
//    //- instantiation template 
//    module CU_FSM(
//        .intr     (),
//        .clk      (),
//        .RST      (),
//        .opcode   (),   // ir[6:0]
//        .pcWrite  (),
//        .regWrite (),
//        .memWE2   (),
//        .memRDEN1 (),
//        .memRDEN2 (),
//        .reset      ()   );
//   
// Dependencies: 
// 
// Revision:
// Revision 1.00 - File Created - 02-01-2020 (from other people's files)
//          1.01 - (02-08-2020) switched states to enum type
//          1.02 - (02-25-2020) made PS assignment blocking
//                              made rst output asynchronous
//          1.03 - (04-24-2020) added "init" state to FSM
//                              changed rst to reset
// 
//////////////////////////////////////////////////////////////////////////////////


module CU_FSM(
    input intr,
    input clk,
    input RST,
    input [2:0] func3,
    input [6:0] opcode,     // ir[6:0]
    output logic pcWrite,
    output logic regWrite,
    output logic memWE2,
    output logic memRDEN1,
    output logic memRDEN2,
    output logic reset,
    output logic csr_we,
    output reg int_taken
  );
    
    typedef enum logic [2:0] {
       st_INIT,
	   st_FET,
       st_EX,
       st_WB,
       st_INTR
    }  state_type; 
    state_type  NS,PS; 
      
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
        OP_RG3 = 7'b0110011,
        OP_INT = 7'b1110011
    } opcode_t;
	opcode_t OPCODE;    //- symbolic names for instruction opcodes
     
	assign OPCODE = opcode_t'(opcode); //- Cast input as enum 
	
	state_type cmd_finish;
	assign cmd_finish = intr ? st_INTR : st_FET;

	//- state registers (PS)
	always @(posedge clk) begin
        if (RST == 1)
            PS <= st_INIT;
        else
            PS <= NS;
    end
    
    always_comb begin              
        //- schedule all outputs to avoid latch
        pcWrite = 1'b0;    
        regWrite = 1'b0;    
        reset = 1'b0;  
		memWE2 = 1'b0;
        memRDEN1 = 1'b0;    
        memRDEN2 = 1'b0;
                       
        case (PS)
            st_INIT: begin
                reset = 1'b1;                    
                NS = st_FET; 
            end

            st_FET: begin
                memRDEN1 = 1'b1;                    
                NS = st_EX; 
            end
              
            st_EX: begin
                pcWrite = 1;
				case (OPCODE)
				    LOAD: begin
                        regWrite = 0;
                        memRDEN2 = 1;
                        NS = st_WB;
                    end
                    
					STORE: begin
                        regWrite = 0;
                        memWE2 = 1;
                        NS = cmd_finish;
                    end
                    
					BRANCH: begin
                        NS = cmd_finish;
                    end
					
					LUI: begin
                        regWrite = 1;
                        NS = cmd_finish;
                    end
                    
                    AUIPC: begin
                        regWrite = 1;
                        NS = cmd_finish;
                    end
					  
					OP_IMM: begin 
                        regWrite = 1;	
                        memRDEN2 = 1;
                        NS = cmd_finish;
                    end
					
					OP_RG3: begin 
                        regWrite = 1;	
                        memRDEN2 = 1;
                        NS = cmd_finish;
                    end
					
	                JAL: begin
					    regWrite = 1; 
					    NS = cmd_finish;
                     end
                    
                    JALR: begin
					    regWrite = 1; 
					    NS = cmd_finish;
                    end
                    
                    OP_INT: if (func3[0]) begin  // csrrw
                        csr_we = 1;
                        regWrite = 1;
                    end else begin  // mret
                        int_taken = 0;
                    end
					 
                    default: begin 
                        regWrite = 0;
                        NS = cmd_finish;
					end
					
                endcase
            end
               
            st_WB: begin
                pcWrite = 0;
                regWrite = 1; 
                memRDEN2 = 1;
                NS = cmd_finish;
            end
            
            st_INTR: begin
                int_taken = 1;
                NS = st_FET;
            end
 
            default: NS = cmd_finish;
           
        endcase //- case statement for FSM states
    end
           
endmodule
