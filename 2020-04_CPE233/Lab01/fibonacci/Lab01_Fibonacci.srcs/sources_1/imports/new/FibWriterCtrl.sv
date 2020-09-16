`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 08:45:36 PM
// Design Name: 
// Module Name: FibWriterCtrl
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


module FibWriterCtrl #(parameter ADDR_WIDTH=4)(
    input en,
    input clk,
    input [ADDR_WIDTH-1:0] last_addr,
    output reg [ADDR_WIDTH-1:0] addr = 0,
    output write
    );
    
    logic reset;
    FibWriterFSM fsm(.en(en), .clk(clk), .fin(addr >= last_addr), .write(write), .reset(reset)); 
    
    always_ff @(posedge clk) begin
        if (reset) 
            addr <= 0;
        else if (write)
            addr <= addr + 1;
    end
    
endmodule
