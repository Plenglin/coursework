`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/25/2020 09:47:54 AM
// Design Name: 
// Module Name: ClockDivider
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


module ClockDivider#(parameter WIDTH=8)(
    input clkin,
    input [WIDTH-1:0] n,
    output clkout
    );
    
    reg [WIDTH-1:0] state = 0;
    reg toggle = 0;
    
    assign clkout = toggle;
    
    always_ff @(posedge clkin) begin
        if (state >= n) begin
            state <= 0;
            toggle = ~toggle;
        end else
            state <= state + 1;
    end
endmodule
