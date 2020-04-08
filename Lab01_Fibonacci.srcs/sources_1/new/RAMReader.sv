`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 10:01:01 PM
// Design Name: 
// Module Name: RAMReader
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


module RAMReader #(parameter ADDR_WIDTH=4)(
    input en,
    input clr,
    input clk,
    output reg [ADDR_WIDTH-1:0] addr = 0
    );
    
    always_ff @(posedge clk) begin
        if (clr) 
            addr <= 0;
        else if (en) 
            addr <= addr + 1;
    end
    
endmodule
