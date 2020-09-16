`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:   Ratner Surf Designs
// Engineer:  James Ratner
// 
// Create Date: 01/16/2020 11:41:46 AM
// Design Name: 
// Module Name: tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: Testbench file for Experiment 3
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module tb( );

   wire [31:0] result; 
   reg [3:0] alu_fun; 
   reg [31:0] srcA,srcB; 
   
ALU  my_alu (
    .alu_fun  (alu_fun),
    .srcA     (srcA),
    .srcB     (srcB),
    .result   (result)
    );
   
   initial 
   begin
   
      alu_fun = 0;     // addition
      srcA = 32'd25; 
      srcB = 32'd26;       
      
      #20             // subtraction
      alu_fun = 8; 
      srcA = 32'd25; 
      srcB = 32'd26;          
      
      #20             // subtraction
      alu_fun = 8; 
      srcA = 32'hFFFFFFFFF; 
      srcB = 32'd1;    
     
       #20             // OR
      alu_fun = 6; 
      srcA = 32'h0000AAAA; 
      srcB = 32'h00005555;   
      
       #20             // AND
      alu_fun = 7; 
      srcA = 32'h0000AAAA; 
      srcB = 32'h00005555;   

       #20             // XOR
      alu_fun = 4; 
      srcA = 32'h0000AAAA; 
      srcB = 32'h00005555;    

       #20             // shift right
      alu_fun = 5; 
      srcA = 32'h0000FF00; 
      srcB = 32'h00000005;   
      
       #20             // shift left
      alu_fun = 1; 
      srcA = 32'h0000FF00; 
      srcB = 32'h00000005;
      
       #20             // shift right arithmetic
      alu_fun = 13; 
      srcA = 32'h8000FF00; 
      srcB = 32'h00000005;          
        
       #20             // set if less than signed
      alu_fun = 2; 
      srcA = 32'h8000FF00; 
      srcB = 32'h00000005;    

       #20             // set if less than unsigned
      alu_fun = 3; 
      srcA = 32'h8000FF00; 
      srcB = 32'h00000005; 
      
       #20             // load upper immediate
      alu_fun = 9; 
      srcA = 32'h0000FF00; 
      srcB = 32'h00000FFF;      
      
      #20             // set if less than unsigned
      alu_fun = 3; 
      srcA = 32'h0000FF00; 
      srcB = 32'h00000FFF;   
     
      #20
      alu_fun = 12;   // undefined operation
   end  

endmodule
