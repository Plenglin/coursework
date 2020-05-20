module dbos_btn();

    logic s_clk;
    logic BTNC;
    logic s_debounced, s_interrupt, s_ps, s_ns;

    DBounce db(
        .clk(s_clk), 
        .sig_in(BTNC), 
        .DB_out(s_debounced)
    );
    
    one_shot_bdir  #(.n(3)) my_oneshot (
        .clk           (s_clk),
        .sig_in        (s_debounced),
        .pos_pulse_out (s_ps),
        .neg_pulse_out (s_ns)
    ); 
        
    one_shot_bdir  #(.n(3)) os2 (
        .clk           (s_clk),
        .sig_in        (s_debounced),
        .pos_pulse_out (s_interrupt)
    ); 
        
    initial begin
        s_clk = 0;
        BTNC = 0;
    end
     
    //- Generate periodic clock signal    
    always begin       
         s_clk = ~s_clk;
         #1;
    end
    
    initial begin
        #100;
        BTNC = 1;
        #100;
        BTNC = 0;
        #3000;
        
        BTNC = 1;
        #500;
        BTNC = 0;
        
    end


endmodule