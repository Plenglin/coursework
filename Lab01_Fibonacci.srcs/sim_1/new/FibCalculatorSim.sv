`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 09:57:08 PM
// Design Name: 
// Module Name: FibCalculatorSim
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


module FibCalculatorSim(

    );
    
    logic calculate;
    logic clk;
    logic ready;
    logic [3:0] addr;
    logic [10:0] disp_num;

    FibCalculator calc(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #0;
    end
    
    initial begin
        calculate = 0;
        #100;
        
        calculate = 1;
        #2;
        calculate = 0;
        #100;
        
        calculate = 1;
        #2;
        calculate = 0;
        #100;
    end
endmodule
