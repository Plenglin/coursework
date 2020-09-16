`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 10:27:31 PM
// Design Name: 
// Module Name: TopSim
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


module TopSim(

    );
    
    logic btn;
    logic clk;
    logic clk_fast;
    logic [3:0] an;
    logic [6:0] seg;
    logic [3:0] led;

    Top top(.*);
    
    always begin
        clk = 1;
        #500;
        clk = 0;
        #500;
    end
    
    always begin
        clk_fast = 1;
        #1;
        clk_fast = 0;
        #1;
    end
    
    initial begin
        btn = 0;
        #20000;
        
        btn = 1;
        #4000;
        btn = 0;
        #60000;
        
        btn = 1;
        #4000;
        btn = 0; 
    end     
       
endmodule
