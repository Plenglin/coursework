`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 06:26:00 PM
// Design Name: 
// Module Name: FibWriter
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


module FibWriter #(parameter DATA_WIDTH=16, ADDR_WIDTH=4) (
    input en,
    input clk,
    input [ADDR_WIDTH-1:0] count,
    output [DATA_WIDTH-1:0] data,
    output [ADDR_WIDTH-1:0] addr,
    output en_write,
    output busy
    );
    
    /**
     *  We need count + 1 states to keep track of this.
     *  States {1...count} represent writing states. State 0 is the
     *  disabled state.
     */
    reg [ADDR_WIDTH:0] state = 0;
    
    FibAccumulator acc(.clk(clk), .en(busy), .clr(state == 1));
    
    assign addr = state[ADDR_WIDTH-1:0] - 1;
    assign data = acc.sum;
    assign busy = state != 0;
    assign en_write = busy;
    assign acc.en = busy;
    
    always_ff @(posedge clk) begin
        if (state == 0 & en)
            state <= 1;
        else if (state == count)
            state <= 0;
        else
            state <= state + 1;
    end
endmodule
