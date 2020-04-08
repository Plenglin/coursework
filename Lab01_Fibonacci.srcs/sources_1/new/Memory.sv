`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 06:02:52 PM
// Design Name: 
// Module Name: Memory
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


module Memory #(parameter DATA_WIDTH=16, ADDR_WIDTH=4, SIZE=16)(
    input [DATA_WIDTH-1:0] data,
    input [ADDR_WIDTH-1:0] addr_write,
    input [ADDR_WIDTH-1:0] addr_read,
    input en_write,
    input clk,
    output [DATA_WIDTH:0] read
    );
    
    reg [DATA_WIDTH-1:0] storage [0:SIZE-1];
    
    assign read = storage[addr_read];
    
    always_ff @(posedge clk) begin
        if (en_write) begin  // Write mode
            storage[addr_write] = data;
        end
    end
    
endmodule
