`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/13/2020 02:58:25 PM
// Design Name: 
// Module Name: test
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


module exp02_test(

    );
    
    logic rst;
    logic pc_write;
    logic [1:0] pc_source;
    logic clk;
    logic [31:0] ir;

    top_level top(.*);
    
    always begin
        clk = 1;
        #1;
        clk = 0;
        #1;
    end
    
    initial begin
        // Stop the counter from incrementing.
        pc_write = 0;
        // Set the instruction source to the next address.
        pc_source = 2'd0;
        // Reset the counter.
        rst = 1;
        #2;
        rst = 0;
        #10;
        
        // Enable the counter. It will increment for 10 cycles.        
        pc_write = 1;
        #20;
        
        // Disable the counter.
        pc_write = 0;
        #6
        
        // Reset the counter. This should happen even if pc_write is low.
        rst = 1;
        #6;
        
        // Re-enable write. The counter should stay at 0.
        pc_write = 1;
        #6;
        
        // Stop resetting.
        rst = 0;
        #10;
        
        // Disable write.
        pc_write = 0;
        #100;
        
        for (int pcs = 1; pcs < 4; pcs++) begin
            // Enable write. Set to source.
            pc_write = 1;
            pc_source = pcs;
            #6;
            
            // Set to increment.
            pc_source = 0;
            #10;
            
            // Stop writing.
            pc_write = 0;
            #8;
            
            // Reset the counter. Should go back to zero.
            rst = 1;
            #6;
            
            // Set source. Should remain at zero.
            pc_source = pcs;
            #6
            
            // Set source to increment. Disable reset. Should not increment.
            pc_source = 0;
            rst = 0;
            #6
            
            // Now it should increment from zero for 10 cycles.
            pc_write = 1;
            #20;
            
            // Disable write.
            pc_write = 0;
            #100;
        end
    end
endmodule
