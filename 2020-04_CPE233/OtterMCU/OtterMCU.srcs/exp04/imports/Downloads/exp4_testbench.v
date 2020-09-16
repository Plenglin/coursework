`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Ratner Surf Designs
// Engineer:  James Ratner
// 
// Create Date: 01/07/2020 12:59:51 PM
// Design Name: 
// Module Name: tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: Testbench file for Exp 4
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module tb_2(); 

   reg rst; 
   reg PCWrite; 
   reg [1:0] pcSource; 
   reg clk; 
   wire [31:0] u_type_imm, s_type_imm; 

top_level my_test(
    .clk  (clk),
    .rst  (rst),
    .PCWrite  (PCWrite),
    .pcSource  (pcSource),
    .u_type_imm (u_type_imm),
    .s_type_imm  (s_type_imm) ); 
  

   //- Generate periodic clock signal    
   initial    
      begin       
         clk = 0;   //- init signal        
         forever  #10 clk = ~clk;    
      end                        
         
   initial        
   begin           
      rst=1;
      PCWrite = 1; 
      pcSource = 0; 
      
      #40
      rst=0;

    end
        
 endmodule
