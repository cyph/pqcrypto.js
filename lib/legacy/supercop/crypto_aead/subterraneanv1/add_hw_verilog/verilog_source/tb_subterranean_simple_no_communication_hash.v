`timescale 1ns / 1ps
module tb_subterranean_simple_no_communication_hash
#(parameter PERIOD = 1000,
maximum_line_length = 10000,
test_memory_file_subterranean_hash = "../data_tests/LWC_HASH_KAT_256.txt"
);

reg [8191:0] test_input_hash;
reg [255:0] test_output_hash;
reg [255:0] true_output_hash;

reg  test_arstn;
reg  test_start_operation;
reg  [3:0] test_operation_type;
reg  [31:0] test_buffer_in;
reg  [1:0] test_buffer_in_size;
wire [31:0] test_buffer_out;
wire test_core_free;
wire test_core_finish;

reg clk;
reg test_error = 1'b0;
reg test_verification = 1'b0;

localparam tb_delay = PERIOD/2;
localparam tb_delay_read = 3*PERIOD/4;

subterranean_simple_no_communication
test (
    .clk(clk),
    .arstn(test_arstn),
    .start_operation(test_start_operation),
    .operation_type(test_operation_type),
    .buffer_in(test_buffer_in),
    .buffer_in_size(test_buffer_in_size),
    .buffer_out(test_buffer_out),
    .core_free(test_core_free),
    .core_finish(test_core_finish)
);

initial begin : clock_generator
    clk <= 1'b1;
    forever begin
        #(PERIOD/2);
        clk <= ~clk;
    end
end

task test_init;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task test_duplex_simple_full;
    input [31:0] sigma;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h1;
        test_buffer_in <= sigma;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task test_duplex_simple_incomplete;
    input [31:0] sigma;
    input [1:0] sigma_size;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h2;
        test_buffer_in <= sigma;
        test_buffer_in_size <= sigma_size;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task test_duplex_encrypt_full;
    input [31:0] sigma;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h3;
        test_buffer_in <= sigma;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task test_duplex_encrypt_incomplete;
    input [31:0] sigma;
    input [1:0] sigma_size;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h4;
        test_buffer_in <= sigma;
        test_buffer_in_size <= sigma_size;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task test_duplex_decrypt_full;
    input [31:0] sigma;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h5;
        test_buffer_in <= sigma;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task test_duplex_decrypt_incomplete;
    input [31:0] sigma;
    input [1:0] sigma_size;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h6;
        test_buffer_in <= sigma;
        test_buffer_in_size <= sigma_size;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task test_duplex_squeeze_simple;
    begin
        test_start_operation <= 1'b1;
        test_operation_type <= 4'h7;
        test_buffer_in <= 32'b01;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
        test_start_operation <= 1'b0;
        test_operation_type <= 4'h0;
        test_buffer_in <= 32'b0;
        test_buffer_in_size <= 2'b11;
        #PERIOD;
    end
endtask

task read_until_get_character;
    input integer file_read;
    input integer character_to_be_found;
    integer temp_text;
    begin
        temp_text = $fgetc(file_read);
        while((temp_text != character_to_be_found) && (!$feof(file_read))) begin
            temp_text = $fgetc(file_read);
        end
    end
endtask

task read_ignore_character;
    input integer file_read;
    input integer character_to_be_ignored;
    output integer last_character;
    integer temp_text;
    begin
        temp_text = $fgetc(file_read);
        while((temp_text == character_to_be_ignored) && (!$feof(file_read))) begin
            temp_text = $fgetc(file_read);
        end
        last_character = temp_text;
    end
endtask

task decode_hex_character;
    input integer a;
    output [3:0] value;
    begin
        if((a >= "0") && (a <= "9")) begin
            value = a - "0";
        end else if((a >= "A") && (a <= "F")) begin
            value = a - "A" + 4'd10;
        end else if((a >= "a") && (a <= "f")) begin
            value = a - "a" + 4'd10;
        end else begin
            value = 4'b0000;
        end
    end
endtask

integer data_file;
integer temp_text1;
integer count;
integer message_size;
integer status_ram_file;
integer test_iterator;
initial begin
    test_arstn <= 1'b0;
    test_start_operation <= 1'b0;
    test_operation_type <= 2'b00;
    test_buffer_in <= 32'b0;
    test_buffer_in_size <= 2'b00;
    test_error <= 1'b0;
    test_verification <= 1'b0;
    test_input_hash <= 8192'b0;
    test_output_hash <= 256'b0;
    true_output_hash <= 256'b0;
    #(PERIOD*2);
    test_arstn <= 1'b1;
    #(PERIOD);
    #(tb_delay);
    data_file = $fopen(test_memory_file_subterranean_hash, "r");
    while(!$feof(data_file)) begin
        read_until_get_character(data_file, "=");
        status_ram_file = $fscanf(data_file, "%d", count);
        #(PERIOD);
        test_error <= 1'b0;
        test_verification <= 1'b0;
        test_output_hash <= 256'b0;
        // Read Message
        read_until_get_character(data_file, "=");
        read_ignore_character(data_file, " ", temp_text1);
        message_size = 0;
        while(temp_text1 != "\n") begin
            decode_hex_character(temp_text1, test_input_hash[7:4]);
            temp_text1 = $fgetc(data_file);
            if(temp_text1 != "\n") begin
                decode_hex_character(temp_text1, test_input_hash[3:0]);
                temp_text1 = $fgetc(data_file);
            end
            test_input_hash = {test_input_hash[7:0], test_input_hash[8191:8]};
            message_size = message_size + 1;
        end
        test_iterator = 0;
        if(message_size > 0) begin
            test_iterator = 1024;
            while (test_iterator > message_size) begin
                test_input_hash = {test_input_hash[7:0], test_input_hash[8191:8]};
                test_iterator = test_iterator - 1;
            end
        end
        // Read MD
        read_until_get_character(data_file, "=");
        read_ignore_character(data_file, " ", temp_text1);
        while(temp_text1 != "\n") begin
            decode_hex_character(temp_text1, true_output_hash[7:4]);
            temp_text1 = $fgetc(data_file);
            if(temp_text1 != "\n") begin
                decode_hex_character(temp_text1, true_output_hash[3:0]);
                temp_text1 = $fgetc(data_file);
            end
            true_output_hash = {true_output_hash[7:0], true_output_hash[255:8]};
        end
        while(test_core_free == 1'b0) begin
            #PERIOD;
        end
        // Start hash procedure
        test_init();
        #(PERIOD);
        test_iterator = 0;
        if (message_size > 0) begin
            while(test_iterator <= message_size - 1) begin
                test_duplex_simple_incomplete({24'b01,test_input_hash[7:0]}, 2'b01);
                test_input_hash <= {test_input_hash[7:0], test_input_hash[8191:8]};
                #(PERIOD);
                test_duplex_simple_incomplete(32'b01, 2'b00);
                #(PERIOD);
                test_iterator = test_iterator + 1;
            end
        end
        test_duplex_simple_incomplete(32'b01, 2'b00);
        #(PERIOD);
        test_duplex_simple_incomplete(32'b01, 2'b00);
        #(PERIOD);
        test_iterator = 0;
        while(test_iterator < 8) begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
            test_iterator = test_iterator + 1;
        end
        #(PERIOD);
        test_iterator = 0;
        while(test_iterator < 32) begin
            test_duplex_squeeze_simple();
            #(PERIOD);
            test_output_hash <= {test_buffer_out, test_output_hash[255:32]};
            test_iterator = test_iterator + 4;
            #(PERIOD);
        end
        #(PERIOD);
        // Compare hash output
        test_verification <= 1'b1;
        if (true_output_hash == test_output_hash) begin
            test_error <= 1'b0;
        end else begin
            test_error <= 1'b1;
            $display("Computed values do not match expected ones");
        end
        #(PERIOD);
        test_error <= 1'b0;
        test_verification <= 1'b0;
        #(PERIOD);
        read_ignore_character(data_file, "\n", temp_text1);
    end
    $fclose(data_file);
    $display("End of the test.");
    disable clock_generator;
    #(PERIOD);
end

initial
begin
    $dumpfile("dump.vcd");
    $dumpvars(1, tb_subterranean_simple_no_communication_hash);
end

endmodule