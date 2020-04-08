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
    output reg write = 0
    );
    
    always_ff @(posedge clk) begin
        if (write) begin
            if (addr >= last_addr)
                write <= 0;
            else
                addr <= addr + 1;
        end else if (en) begin
            write <= 1;
            addr <= 0;
        end
    end
    
    
endmodule
