`timescale 1ns / 1ps
module tb_subterranean_simple_no_communication_aead
#(parameter PERIOD = 1000,
maximum_line_length = 10000,
test_memory_file_subterranean_aead = "../data_tests/LWC_AEAD_KAT_128_128.txt"
);

reg [127:0] test_input_key_enc;
reg [127:0]  test_input_nonce_enc;
reg [255:0] test_input_pt_enc;
reg [255:0] test_input_ad_enc;
reg [383:0] test_output_ct_enc;
reg [383:0] true_output_ct_enc;

reg [127:0] test_input_key_dec;
reg [127:0]  test_input_nonce_dec;
reg [255:0] test_input_ct_dec;
reg [255:0] test_input_ad_dec;
reg [383:0] test_output_pt_dec;
reg [383:0] true_output_pt_dec;

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
integer key_size;
integer nonce_size;
integer pt_size;
integer ad_size;
integer ct_size;
integer tag_size;
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
    test_input_key_enc <= 128'b0;
    test_input_nonce_enc <= 128'b0;
    test_input_pt_enc <= 256'b0;
    test_input_ad_enc <= 256'b0;
    test_output_ct_enc <= 384'b0;
    true_output_ct_enc <= 384'b0;
    test_input_key_dec <= 128'b0;
    test_input_nonce_dec <= 128'b0;
    test_input_ct_dec <= 256'b0;
    test_input_ad_dec <= 256'b0;
    test_output_pt_dec <= 384'b0;
    true_output_pt_dec <= 384'b0;
    tag_size <= 16;
    #(PERIOD*2);
    test_arstn <= 1'b1;
    #(PERIOD);
    #(tb_delay);
    data_file = $fopen(test_memory_file_subterranean_aead, "r");
    while(!$feof(data_file)) begin
        read_until_get_character(data_file, "=");
        status_ram_file = $fscanf(data_file, "%d", count);
        #(PERIOD);
        test_error <= 1'b0;
        test_verification <= 1'b0;
        test_input_key_enc <= 128'b0;
        test_input_nonce_enc <= 128'b0;
        test_input_pt_enc <= 256'b0;
        test_input_ad_enc <= 256'b0;
        test_output_ct_enc <= 384'b0;
        true_output_ct_enc <= 384'b0;
        test_input_key_dec <= 128'b0;
        test_input_nonce_dec <= 128'b0;
        test_input_ct_dec <= 256'b0;
        test_input_ad_dec <= 256'b0;
        test_output_pt_dec <= 384'b0;
        true_output_pt_dec <= 384'b0;
        #(PERIOD);
        // Read key
        read_until_get_character(data_file, "=");
        read_ignore_character(data_file, " ", temp_text1);
        key_size = 0;
        while(temp_text1 != "\n") begin
            decode_hex_character(temp_text1, test_input_key_enc[7:4]);
            temp_text1 = $fgetc(data_file);
            if(temp_text1 != "\n") begin
                decode_hex_character(temp_text1, test_input_key_enc[3:0]);
                temp_text1 = $fgetc(data_file);
            end
            test_input_key_enc = {test_input_key_enc[7:0], test_input_key_enc[127:8]};
            key_size = key_size + 1;
        end
        // Read nonce
        read_until_get_character(data_file, "=");
        read_ignore_character(data_file, " ", temp_text1);
        nonce_size = 0;
        while(temp_text1 != "\n") begin
            decode_hex_character(temp_text1, test_input_nonce_enc[7:4]);
            temp_text1 = $fgetc(data_file);
            if(temp_text1 != "\n") begin
                decode_hex_character(temp_text1, test_input_nonce_enc[3:0]);
                temp_text1 = $fgetc(data_file);
            end
            test_input_nonce_enc = {test_input_nonce_enc[7:0], test_input_nonce_enc[127:8]};
            nonce_size = nonce_size + 1;
        end
        // Read PT
        read_until_get_character(data_file, "=");
        read_ignore_character(data_file, " ", temp_text1);
        pt_size = 0;
        while(temp_text1 != "\n") begin
            decode_hex_character(temp_text1, test_input_pt_enc[7:4]);
            temp_text1 = $fgetc(data_file);
            if(temp_text1 != "\n") begin
                decode_hex_character(temp_text1, test_input_pt_enc[3:0]);
                temp_text1 = $fgetc(data_file);
            end
            test_input_pt_enc = {test_input_pt_enc[7:0], test_input_pt_enc[255:8]};
            true_output_pt_dec = {test_input_pt_enc[255:248], true_output_pt_dec[383:8]};
            pt_size = pt_size + 1;
        end
        // If the length is variable the buffer has to be adjusted
        if(pt_size > 0) begin
            test_iterator = 32;
            while (test_iterator > pt_size) begin
                test_input_pt_enc = {test_input_pt_enc[7:0], test_input_pt_enc[255:8]};
                test_iterator = test_iterator - 1;
            end
        end
        // Read AD
        read_until_get_character(data_file, "=");
        read_ignore_character(data_file, " ", temp_text1);
        ad_size = 0;
        while(temp_text1 != "\n") begin
            decode_hex_character(temp_text1, test_input_ad_enc[7:4]);
            temp_text1 = $fgetc(data_file);
            if(temp_text1 != "\n") begin
                decode_hex_character(temp_text1, test_input_ad_enc[3:0]);
                temp_text1 = $fgetc(data_file);
            end
            test_input_ad_enc = {test_input_ad_enc[7:0], test_input_ad_enc[255:8]};
            ad_size = ad_size + 1;
        end
        // If the length is variable the buffer has to be adjusted
        if(ad_size > 0) begin
            test_iterator = 32;
            while (test_iterator > ad_size) begin
                test_input_ad_enc = {test_input_ad_enc[7:0], test_input_ad_enc[255:8]};
                test_iterator = test_iterator - 1;
            end
        end
        // Read CT
        read_until_get_character(data_file, "=");
        read_ignore_character(data_file, " ", temp_text1);
        ct_size = 0;
        while(temp_text1 != "\n") begin
            decode_hex_character(temp_text1, true_output_ct_enc[7:4]);
            temp_text1 = $fgetc(data_file);
            if(temp_text1 != "\n") begin
                decode_hex_character(temp_text1, true_output_ct_enc[3:0]);
                temp_text1 = $fgetc(data_file);
            end
            true_output_ct_enc = {true_output_ct_enc[7:0], true_output_ct_enc[383:8]};
            if(ct_size >= pt_size) begin
                true_output_pt_dec = {true_output_ct_enc[383:376], true_output_pt_dec[383:8]};
            end
            ct_size = ct_size + 1;
        end
        // If the length is variable the buffer has to be adjusted
        if(ct_size > 0) begin
            test_iterator = 48;
            while (test_iterator > ct_size) begin
                true_output_ct_enc = {true_output_ct_enc[7:0], true_output_ct_enc[383:8]};
                true_output_pt_dec = {true_output_pt_dec[7:0], true_output_pt_dec[383:8]};
                test_iterator = test_iterator - 1;
            end
        end
        test_input_key_dec = test_input_key_enc;
        test_input_nonce_dec = test_input_nonce_enc;
        test_input_ad_dec = test_input_ad_enc;
        test_input_ct_dec = true_output_ct_enc[255:0];
        while(test_core_free == 1'b0) begin
            #PERIOD;
        end
        // Start the encryption procedure
        // Initialize the state
        test_init();
        #(PERIOD);
        // Absorb the key
        test_iterator = 0;
        if (key_size >= 4) begin
            while(test_iterator <= key_size - 4) begin
                test_duplex_simple_full(test_input_key_enc[31:0]);
                test_input_key_enc <= {test_input_key_enc[31:0], test_input_key_enc[127:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((key_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_key_enc[7:0]}, 2'b01);
            test_input_key_enc <= {test_input_key_enc[7:0], test_input_key_enc[127:8]};
            #(PERIOD);
        end else if((key_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_key_enc[15:0]}, 2'b10);
            test_input_key_enc <= {test_input_key_enc[15:0], test_input_nonce_enc[127:16]};
            #(PERIOD);
        end else if((key_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_key_enc[23:0]}, 2'b11);
            test_input_key_enc <= {test_input_key_enc[23:0], test_input_key_enc[127:24]};
            #(PERIOD);
        end else begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        #(PERIOD);
        // Absorb the Nonce
        test_iterator = 0;
        if (nonce_size >= 4) begin
            while(test_iterator <= nonce_size - 4) begin
                test_duplex_simple_full(test_input_nonce_enc[31:0]);
                test_input_nonce_enc <= {test_input_nonce_enc[31:0], test_input_nonce_enc[127:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((nonce_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_nonce_enc[7:0]}, 2'b01);
            test_input_nonce_enc <= {test_input_nonce_enc[7:0], test_input_nonce_enc[127:8]};
            #(PERIOD);
        end else if((nonce_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_nonce_enc[15:0]}, 2'b10);
            test_input_nonce_enc <= {test_input_nonce_enc[15:0], test_input_nonce_enc[127:16]};
            #(PERIOD);
        end else if((nonce_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_nonce_enc[23:0]}, 2'b11);
            test_input_nonce_enc <= {test_input_nonce_enc[23:0], test_input_nonce_enc[127:24]};
            #(PERIOD);
        end else begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        // Perform Blank
        #(PERIOD);
        test_iterator = 0;
        while(test_iterator < 8) begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
            test_iterator = test_iterator + 1;
        end
        // Absorb the AD
        test_iterator = 0;
        if (ad_size >= 4) begin
            while(test_iterator <= ad_size - 4) begin
                test_duplex_simple_full(test_input_ad_enc[31:0]);
                test_input_ad_enc <= {test_input_ad_enc[31:0], test_input_ad_enc[255:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((ad_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_ad_enc[7:0]}, 2'b01);
            test_input_ad_enc <= {test_input_ad_enc[7:0], test_input_ad_enc[255:8]};
            #(PERIOD);
        end else if((ad_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_ad_enc[15:0]}, 2'b10);
            test_input_ad_enc <= {test_input_ad_enc[15:0], test_input_ad_enc[255:16]};
            #(PERIOD);
        end else if((ad_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_ad_enc[23:0]}, 2'b11);
            test_input_ad_enc <= {test_input_ad_enc[23:0], test_input_ad_enc[255:24]};
            #(PERIOD);
        end else begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        // Absorb the pt and encrypt
        test_iterator = 0;
        if (pt_size >= 4) begin
            while(test_iterator <= pt_size - 4) begin
                test_duplex_encrypt_full(test_input_pt_enc[31:0]);
                test_input_pt_enc <= {test_input_pt_enc[31:0], test_input_pt_enc[255:32]};
                #(PERIOD);
                test_output_ct_enc <= {test_buffer_out, test_output_ct_enc[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        if((pt_size - test_iterator) == 1) begin
            test_duplex_encrypt_incomplete({24'b01, test_input_pt_enc[7:0]}, 2'b01);
            test_input_pt_enc <= {test_input_pt_enc[7:0], test_input_pt_enc[255:8]};
            #(PERIOD);
            test_output_ct_enc <= {test_buffer_out[7:0], test_output_ct_enc[383:8]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 2) begin
            test_duplex_encrypt_incomplete({16'b01,test_input_pt_enc[15:0]}, 2'b10);
            test_input_pt_enc <= {test_input_pt_enc[15:0], test_input_pt_enc[255:16]};
            #(PERIOD);
            test_output_ct_enc <= {test_buffer_out[15:0], test_output_ct_enc[383:16]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 3) begin
            test_duplex_encrypt_incomplete({8'b01,test_input_pt_enc[23:0]}, 2'b11);
            test_input_pt_enc <= {test_input_pt_enc[23:0], test_input_pt_enc[255:24]};
            #(PERIOD);
            test_output_ct_enc <= {test_buffer_out[23:0], test_output_ct_enc[383:24]};
            #(PERIOD);
        end else begin
            test_duplex_encrypt_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        // Perform Blank
        #(PERIOD);
        test_iterator = 0;
        while(test_iterator < 8) begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
            test_iterator = test_iterator + 1;
        end
        // Squeeze Tag
        #(PERIOD);
        test_iterator = 0;
        if (tag_size > 4) begin
            while(test_iterator < tag_size - 4) begin
                test_duplex_squeeze_simple();
                #(PERIOD);
                test_output_ct_enc <= {test_buffer_out, test_output_ct_enc[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        test_duplex_squeeze_simple();
        #(PERIOD);
        if((tag_size - test_iterator) == 1) begin
            test_output_ct_enc <= {test_buffer_out[7:0], test_output_ct_enc[383:8]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 2) begin
            test_output_ct_enc <= {test_buffer_out[15:0], test_output_ct_enc[383:16]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 3) begin
            test_output_ct_enc <= {test_buffer_out[23:0], test_output_ct_enc[383:24]};
            #(PERIOD);
        end else begin
            test_output_ct_enc <= {test_buffer_out[31:0], test_output_ct_enc[383:32]};
            #(PERIOD);
        end
        test_iterator = 48;
        while (test_iterator > ct_size) begin
            test_output_ct_enc = {test_output_ct_enc[7:0], test_output_ct_enc[383:8]};
            test_iterator = test_iterator - 1;
        end
        // Check ciphertext and tag
        #(PERIOD);
        test_verification <= 1'b1;
        if (true_output_ct_enc == test_output_ct_enc) begin
            test_error <= 1'b0;
        end else begin
            test_error <= 1'b1;
            $display("Computed values do not match expected ones");
        end
        #(PERIOD);
        test_error <= 1'b0;
        test_verification <= 1'b0;
        #(PERIOD);
        // Start the decryption procedure
        // Initialize the state
        test_init();
        #(PERIOD);
        // Absorb the key
        test_iterator = 0;
        if (key_size >= 4) begin
            while(test_iterator <= key_size - 4) begin
                test_duplex_simple_full(test_input_key_dec[31:0]);
                test_input_key_dec <= {test_input_key_dec[31:0], test_input_key_dec[127:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((key_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_key_dec[7:0]}, 2'b01);
            test_input_key_dec <= {test_input_key_dec[7:0], test_input_key_dec[127:8]};
            #(PERIOD);
        end else if((key_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_key_dec[15:0]}, 2'b10);
            test_input_key_dec <= {test_input_key_dec[15:0], test_input_key_dec[127:16]};
            #(PERIOD);
        end else if((key_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_key_dec[23:0]}, 2'b11);
            test_input_key_dec <= {test_input_key_dec[23:0], test_input_key_dec[127:24]};
            #(PERIOD);
        end else begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        // Absorb the Nonce
        test_iterator = 0;
        if (nonce_size >= 4) begin
            while(test_iterator <= nonce_size - 4) begin
                test_duplex_simple_full(test_input_nonce_dec[31:0]);
                test_input_nonce_dec <= {test_input_nonce_dec[31:0], test_input_nonce_dec[127:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((nonce_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_nonce_dec[7:0]}, 2'b01);
            test_input_nonce_dec <= {test_input_nonce_dec[7:0], test_input_nonce_dec[127:8]};
            #(PERIOD);
        end else if((nonce_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_nonce_dec[15:0]}, 2'b10);
            test_input_nonce_dec <= {test_input_nonce_dec[15:0], test_input_nonce_dec[127:16]};
            #(PERIOD);
        end else if((nonce_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_nonce_dec[23:0]}, 2'b11);
            test_input_nonce_dec <= {test_input_nonce_dec[23:0], test_input_nonce_dec[127:24]};
            #(PERIOD);
        end else begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        // Perform Blank
        #(PERIOD);
        test_iterator = 0;
        while(test_iterator < 8) begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
            test_iterator = test_iterator + 1;
        end
        // Absorb the AD
        #(PERIOD);
        test_iterator = 0;
        if (ad_size >= 4) begin
            while(test_iterator <= ad_size - 4) begin
                test_duplex_simple_full(test_input_ad_dec[31:0]);
                test_input_ad_dec <= {test_input_ad_dec[31:0], test_input_ad_dec[255:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((ad_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_ad_dec[7:0]}, 2'b01);
            test_input_ad_dec <= {test_input_ad_dec[7:0], test_input_ad_dec[255:8]};
            #(PERIOD);
        end else if((ad_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_ad_dec[15:0]}, 2'b10);
            test_input_ad_dec <= {test_input_ad_dec[15:0], test_input_ad_dec[255:16]};
            #(PERIOD);
        end else if((ad_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_ad_dec[23:0]}, 2'b11);
            test_input_ad_dec <= {test_input_ad_dec[23:0], test_input_ad_dec[255:24]};
            #(PERIOD);
        end else begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        // Absorb the ct and decrypt
        #(PERIOD);
        test_iterator = 0;
        if (pt_size >= 4) begin
            while(test_iterator <= pt_size - 4) begin
                test_duplex_decrypt_full(test_input_ct_dec[31:0]);
                test_input_ct_dec <= {test_input_ct_dec[31:0], test_input_ct_dec[255:32]};
                #(PERIOD);
                test_output_pt_dec <= {test_buffer_out, test_output_pt_dec[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        if((pt_size - test_iterator) == 1) begin
            test_duplex_decrypt_incomplete({24'b01, test_input_ct_dec[7:0]}, 2'b01);
            test_input_ct_dec <= {test_input_ct_dec[7:0], test_input_ct_dec[255:8]};
            #(PERIOD);
            test_output_pt_dec <= {test_buffer_out[7:0], test_output_pt_dec[383:8]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 2) begin
            test_duplex_decrypt_incomplete({16'b01,test_input_ct_dec[15:0]}, 2'b10);
            test_input_ct_dec <= {test_input_ct_dec[15:0], test_input_ct_dec[255:16]};
            #(PERIOD);
            test_output_pt_dec <= {test_buffer_out[15:0], test_output_pt_dec[383:16]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 3) begin
            test_duplex_decrypt_incomplete({8'b01,test_input_ct_dec[23:0]}, 2'b11);
            test_input_ct_dec <= {test_input_ct_dec[23:0], test_input_ct_dec[255:24]};
            #(PERIOD);
            test_output_pt_dec <= {test_buffer_out[23:0], test_output_pt_dec[383:24]};
            #(PERIOD);
        end else begin
            test_duplex_decrypt_incomplete(32'b01, 2'b00);
            #(PERIOD);
        end
        // Perform Blank
        #(PERIOD);
        test_iterator = 0;
        while(test_iterator < 8) begin
            test_duplex_simple_incomplete(32'b01, 2'b00);
            #(PERIOD);
            test_iterator = test_iterator + 1;
        end
        #(PERIOD);
        // Squeeze Tag
        test_iterator = 0;
        if (tag_size > 4) begin
            while(test_iterator < tag_size - 4) begin
                test_duplex_squeeze_simple();
                #(PERIOD);
                test_output_pt_dec <= {test_buffer_out, test_output_pt_dec[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        test_duplex_squeeze_simple();
        #(PERIOD);
        if((tag_size - test_iterator) == 1) begin
            test_output_pt_dec <= {test_buffer_out[7:0], test_output_pt_dec[383:8]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 2) begin
            test_output_pt_dec <= {test_buffer_out[15:0], test_output_pt_dec[383:16]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 3) begin
            test_output_pt_dec <= {test_buffer_out[23:0], test_output_pt_dec[383:24]};
            #(PERIOD);
        end else begin
            test_output_pt_dec <= {test_buffer_out[31:0], test_output_pt_dec[383:32]};
            #(PERIOD);
        end
        test_iterator = 48;
        while (test_iterator > ct_size) begin
            test_output_pt_dec = {test_output_pt_dec[7:0], test_output_pt_dec[383:8]};
            test_iterator = test_iterator - 1;
        end
        // Check plaintext and tag
        #(PERIOD);
        test_verification <= 1'b1;
        if (true_output_pt_dec == test_output_pt_dec) begin
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
    $dumpvars(1, tb_subterranean_simple_no_communication_aead);
end

endmodule