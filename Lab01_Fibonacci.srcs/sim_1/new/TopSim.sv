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
    logic [3:0] an;
    logic [6:0] seg;
    logic [3:0] led;

    Top top(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #1;
    end
    
    initial begin
        btn = 0;
        #20;
        
        btn = 1;
        #4;
        btn = 0;
        #60;
        
        btn = 1;
        #4;
        btn = 0; 
    end     
       
endmodule
