`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/07/2020 09:22:11 PM
// Design Name: 
// Module Name: MemorySim
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


module MemorySim(

    );
    logic [7:0] data;
    logic [7:0] addr_write;
    logic [7:0] addr_read = 0;
    logic write;
    logic clk;
    logic [7:0] read;
   
    Memory #(.DATA_WIDTH(8), .ADDR_WIDTH(8), .SIZE(256)) mem(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #1;
    end
    
    initial begin
        #10;
        
        data = 8'had;
        addr_write = 8'h32;
        write = 1;
        #2;
        write = 0;
        #10;
        
        data = 8'hbe;
        addr_write = 8'h82;
        write = 1;
        #2;
        write = 0;
        #10;
        
        data = 8'hde;
        addr_write = 8'ha1;
        write = 1;
        #2;
        write = 0;
        #10;
        
        data = 8'h69;
        #10;
        
        data = 8'hef;
        addr_write = 8'hff;
        write = 1;
        #2;
        write = 0;
        #30;
        
        addr_read = 8'ha1;
        #2; 
        addr_read = 8'h32;
        #2; 
        addr_read = 8'h82;
        #2; 
        addr_read = 8'hff;
        #2;        
        addr_read = 8'h0a;
        #2; 
        addr_read = 8'ha2;
        #2; 
        addr_read = 8'h76;
        #2; 
        
    end
    

endmodule
