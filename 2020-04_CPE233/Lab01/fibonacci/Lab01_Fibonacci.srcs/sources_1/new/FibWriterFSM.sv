`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/09/2020 07:53:04 PM
// Design Name: 
// Module Name: FibWriterFSM
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


module FibWriterFSM(
    input en,
    input fin,
    input clk,
    output write,
    output reset
    );
    
    reg state = 0;
    assign write = state;
    assign reset = ~state;
    
    always_ff @(posedge clk) begin
        if (state) begin
            if (fin) begin
                state <= 0;
            end
        end else begin
            if (en) begin
                state <= 1;
            end
        end 
    end
    
endmodule
