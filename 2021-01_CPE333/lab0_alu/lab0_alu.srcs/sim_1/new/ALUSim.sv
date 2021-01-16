`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/05/2021 08:38:40 AM
// Design Name: 
// Module Name: ALUSim
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


module ALUSim(

    );
    
    logic [31:0] a, b, result;
    logic [2:0] op;
    logic carry_out;
    ALU alu(.a(a), .b(b), .aluop(op), .carry_out(carry_out), .result(result));
    
    initial begin 
        assign a = 32'd5;
        assign b = 32'd8;
        assign op = 4'b010;
        #1 
        assert(result === 32'd13);
        assert(carry_out === 0);
        
        #4
        
        assign a = 32'hffffffff;
        assign b = 32'hffffffff;
        assign op = 4'b010;
        #1 
        assert(carry_out === 1);
    
        #4

        assign a = 32'd8;
        assign b = 32'd5;
        assign op = 4'b110;
        #1 
        assert(result === 32'd3);
        assert(carry_out === 0);
    
        #4
        
        assign a = 32'b1100;
        assign b = 32'b0110;
        assign op = 4'b000;
        #1 
        assert(result === 32'b0100);
        assert(carry_out === 0);

        #4
        
        assign a = 32'b1100;
        assign b = 32'b0110;
        assign op = 4'b001;
        #1 
        assert(result === 32'b1110);
        assert(carry_out === 0);
        
        #4
        
        assign a = 32'd423;
        assign b = 32'd234;
        assign op = 4'b111;
        #1 
        assert(result === 32'b0);
        assert(carry_out === 0);
    end
endmodule
