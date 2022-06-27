`timescale 1ns / 1ps
module tb_subterranean_simple_axi4_lite_aead
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

reg test_aresetn;
reg [7:0] test_s_axi_awaddr;
reg [2:0] test_s_axi_awprot;
reg test_s_axi_awvalid;
wire test_s_axi_awready;
reg [31:0] test_s_axi_wdata;
reg [3:0]  test_s_axi_wstrb;
reg test_s_axi_wvalid;
wire test_s_axi_wready;
wire [1:0] test_s_axi_bresp;
wire test_s_axi_bvalid;
reg test_s_axi_bready;
reg [7:0] test_s_axi_araddr;
reg [2:0] test_s_axi_arprot;
reg test_s_axi_arvalid;
wire test_s_axi_arready;
wire [31:0] test_s_axi_rdata;
wire [1:0]  test_s_axi_rresp;
wire test_s_axi_rvalid;
reg test_s_axi_rready;

reg [31:0] test_temp_buffer_out;

reg clk;
reg test_error = 1'b0;
reg test_verification = 1'b0;

localparam tb_delay = PERIOD/2;
localparam tb_delay_read = 3*PERIOD/4;

initial begin : clock_generator
    clk <= 1'b1;
    forever begin
        #(PERIOD/2);
        clk <= ~clk;
    end
end

subterranean_simple_axi4_lite
test (
    .aclk(clk),
    .aresetn(test_aresetn),
    .s_axi_awaddr(test_s_axi_awaddr),
    .s_axi_awprot(test_s_axi_awprot),
    .s_axi_awvalid(test_s_axi_awvalid),
    .s_axi_awready(test_s_axi_awready),
    .s_axi_wdata(test_s_axi_wdata),
    .s_axi_wstrb(test_s_axi_wstrb),
    .s_axi_wvalid(test_s_axi_wvalid),
    .s_axi_wready(test_s_axi_wready),
    .s_axi_bresp(test_s_axi_bresp),
    .s_axi_bvalid(test_s_axi_bvalid),
    .s_axi_bready(test_s_axi_bready),
    .s_axi_araddr(test_s_axi_araddr),
    .s_axi_arprot(test_s_axi_arprot),
    .s_axi_arvalid(test_s_axi_arvalid),
    .s_axi_arready(test_s_axi_arready),
    .s_axi_rdata(test_s_axi_rdata),
    .s_axi_rresp(test_s_axi_rresp),
    .s_axi_rvalid(test_s_axi_rvalid),
    .s_axi_rready(test_s_axi_rready)
);

task axi_4_lite_write_value;
    input [7:0] address;
    input [31:0] value;
    input [1:0] value_size;
    begin
        test_s_axi_awaddr <= 8'h00;
        test_s_axi_awprot <= 3'b000;
        test_s_axi_awvalid <= 1'b0;
        test_s_axi_wdata <= 32'b0;
        test_s_axi_wstrb <= 4'b0000;
        test_s_axi_wvalid <= 1'b0;
        test_s_axi_bready <= 1'b0;
        test_s_axi_araddr <= 8'h00;
        test_s_axi_arprot <= 3'b000;
        test_s_axi_arvalid <= 1'b0;
        test_s_axi_rready <= 1'b0;
        #(PERIOD);
        test_s_axi_awaddr <= address;
        test_s_axi_awprot <= 3'b000;
        test_s_axi_awvalid <= 1'b1;
        test_s_axi_wdata <= value;
        case(value_size)
            2'b00 : begin
                test_s_axi_wstrb <= 4'b0001;
            end
            2'b01 : begin
                test_s_axi_wstrb <= 4'b0011;
            end
            2'b10 : begin
                test_s_axi_wstrb <= 4'b0111;
            end
            2'b11 : begin
                test_s_axi_wstrb <= 4'b1111;
            end
        endcase
        test_s_axi_wvalid <= 1'b1;
        if((test_s_axi_awvalid != 1'b1) || (test_s_axi_awready != 1'b1)) begin
            #(PERIOD);
        end
        if((test_s_axi_wvalid != 1'b1) || (test_s_axi_wready != 1'b1)) begin
            #(PERIOD);
        end
        test_s_axi_awaddr <= 8'h00;
        test_s_axi_awprot <= 3'b000;
        test_s_axi_awvalid <= 1'b0;
        test_s_axi_wdata <= 32'b0;
        test_s_axi_wstrb <= 4'b0000;
        test_s_axi_wvalid <= 1'b0;
        test_s_axi_bready <= 1'b1;
        if((test_s_axi_bvalid != 1'b1) || (test_s_axi_bready != 1'b1)) begin
            #(PERIOD);
        end
        #(PERIOD);
        test_s_axi_bready <= 1'b0;
        #(PERIOD);
    end
endtask

task axi_4_lite_read_value;
    input [7:0] address;
    output [31:0] value;
    begin
        test_s_axi_awaddr <= 8'h00;
        test_s_axi_awprot <= 3'b000;
        test_s_axi_awvalid <= 1'b0;
        test_s_axi_wdata <= 32'b0;
        test_s_axi_wstrb <= 4'b0000;
        test_s_axi_wvalid <= 1'b0;
        test_s_axi_bready <= 1'b0;
        test_s_axi_araddr <= 8'h00;
        test_s_axi_arprot <= 3'b000;
        test_s_axi_arvalid <= 1'b0;
        test_s_axi_rready <= 1'b0;
        #(PERIOD);
        test_s_axi_araddr <= address;
        test_s_axi_arprot <= 3'b000;
        test_s_axi_arvalid <= 1'b1;
        if((test_s_axi_arvalid != 1'b1) || (test_s_axi_arready != 1'b1)) begin
            #(PERIOD);
        end
        test_s_axi_araddr <= 8'h00;
        test_s_axi_arprot <= 3'b000;
        test_s_axi_arvalid <= 1'b0;
        test_s_axi_rready <= 1'b1;
        if((test_s_axi_rvalid != 1'b1) || (test_s_axi_rready != 1'b1)) begin
            #(PERIOD);
        end
        #(PERIOD);
        test_s_axi_rready <= 1'b0;
        value <= test_s_axi_rdata;
        #(PERIOD);
    end
endtask

task test_init;
    begin
        axi_4_lite_write_value(8'h00, 32'b0, 2'b11);
    end
endtask

task test_duplex_simple_full;
    input [31:0] sigma;
    begin
        axi_4_lite_write_value(8'h10, sigma, 2'b11);
    end
endtask

task test_duplex_simple_incomplete;
    input [31:0] sigma;
    input [1:0] sigma_size;
    begin
        axi_4_lite_write_value({4'h2,sigma_size,2'b00}, sigma, 2'b11);
    end
endtask

task test_duplex_encrypt_full;
    input [31:0] sigma;
    begin
        axi_4_lite_write_value(8'h30, sigma, 2'b11);
    end
endtask

task test_duplex_encrypt_incomplete;
    input [31:0] sigma;
    input [1:0] sigma_size;
    begin
        axi_4_lite_write_value({4'h4,sigma_size,2'b00}, sigma, 2'b11);
    end
endtask

task test_duplex_decrypt_full;
    input [31:0] sigma;
    begin
        axi_4_lite_write_value(8'h50, sigma, 2'b11);
    end
endtask

task test_duplex_decrypt_incomplete;
    input [31:0] sigma;
    input [1:0] sigma_size;
    begin
        axi_4_lite_write_value({4'h6, sigma_size,2'b00}, sigma, 2'b11);
    end
endtask

task test_duplex_squeeze_simple;
    begin
        axi_4_lite_write_value(8'h70, 32'b01, 2'b11);
    end
endtask

task test_read_buffer;
    output [31:0] value;
    begin
        axi_4_lite_read_value(8'h80, value);
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
    test_aresetn <= 1'b0;
    test_s_axi_awaddr <= 8'h00;
    test_s_axi_awprot <= 3'b000;
    test_s_axi_awvalid <= 1'b0;
    test_s_axi_wdata <= 32'b0;
    test_s_axi_wstrb <= 4'b0000;
    test_s_axi_wvalid <= 1'b0;
    test_s_axi_bready <= 1'b0;
    test_s_axi_araddr <= 8'h00;
    test_s_axi_arprot <= 3'b000;
    test_s_axi_arvalid <= 1'b0;
    test_s_axi_rready <= 1'b0;
    
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
    test_aresetn <= 1'b1;
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
            test_input_key_enc <= {test_input_key_enc[15:0], test_input_key_enc[127:16]};
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
        #(PERIOD);
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
        #(PERIOD);
        test_iterator = 0;
        if (pt_size >= 4) begin
            while(test_iterator <= pt_size - 4) begin
                test_duplex_encrypt_full(test_input_pt_enc[31:0]);
                test_input_pt_enc <= {test_input_pt_enc[31:0], test_input_pt_enc[255:32]};
                #(PERIOD);
                test_read_buffer(test_temp_buffer_out);
                test_output_ct_enc <= {test_temp_buffer_out, test_output_ct_enc[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        if((pt_size - test_iterator) == 1) begin
            test_duplex_encrypt_incomplete({24'b01, test_input_pt_enc[7:0]}, 2'b01);
            test_input_pt_enc <= {test_input_pt_enc[7:0], test_input_pt_enc[255:8]};
            #(PERIOD);
            test_read_buffer(test_temp_buffer_out);
            test_output_ct_enc <= {test_temp_buffer_out[7:0], test_output_ct_enc[383:8]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 2) begin
            test_duplex_encrypt_incomplete({16'b01,test_input_pt_enc[15:0]}, 2'b10);
            test_input_pt_enc <= {test_input_pt_enc[15:0], test_input_pt_enc[255:16]};
            #(PERIOD);
            test_read_buffer(test_temp_buffer_out);
            test_output_ct_enc <= {test_temp_buffer_out[15:0], test_output_ct_enc[383:16]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 3) begin
            test_duplex_encrypt_incomplete({8'b01,test_input_pt_enc[23:0]}, 2'b11);
            test_input_pt_enc <= {test_input_pt_enc[23:0], test_input_pt_enc[255:24]};
            #(PERIOD);
            test_read_buffer(test_temp_buffer_out);
            test_output_ct_enc <= {test_temp_buffer_out[23:0], test_output_ct_enc[383:24]};
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
        #(PERIOD);
        // Squeeze Tag
        test_iterator = 0;
        if (tag_size > 4) begin
            while(test_iterator < tag_size - 4) begin
                test_duplex_squeeze_simple();
                #(PERIOD);
                test_read_buffer(test_temp_buffer_out);
                test_output_ct_enc <= {test_temp_buffer_out, test_output_ct_enc[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        test_duplex_squeeze_simple();
        #(PERIOD);
        if((tag_size - test_iterator) == 1) begin
            test_read_buffer(test_temp_buffer_out);
            test_output_ct_enc <= {test_temp_buffer_out[7:0], test_output_ct_enc[383:8]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 2) begin
            test_read_buffer(test_temp_buffer_out);
            test_output_ct_enc <= {test_temp_buffer_out[15:0], test_output_ct_enc[383:16]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 3) begin
            test_read_buffer(test_temp_buffer_out);
            test_output_ct_enc <= {test_temp_buffer_out[23:0], test_output_ct_enc[383:24]};
            #(PERIOD);
        end else begin
            test_read_buffer(test_temp_buffer_out);
            test_output_ct_enc <= {test_temp_buffer_out[31:0], test_output_ct_enc[383:32]};
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
                test_input_key_dec = {test_input_key_dec[31:0], test_input_key_dec[127:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((key_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_key_dec[7:0]}, 2'b01);
            test_input_key_dec = {test_input_key_dec[7:0], test_input_key_dec[127:8]};
            #(PERIOD);
        end else if((key_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_key_dec[15:0]}, 2'b10);
            test_input_key_dec = {test_input_key_dec[15:0], test_input_key_dec[127:16]};
            #(PERIOD);
        end else if((key_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_key_dec[23:0]}, 2'b11);
            test_input_key_dec = {test_input_key_dec[23:0], test_input_key_dec[127:24]};
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
                test_duplex_simple_full(test_input_nonce_dec[31:0]);
                test_input_nonce_dec = {test_input_nonce_dec[31:0], test_input_nonce_dec[127:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((nonce_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_nonce_dec[7:0]}, 2'b01);
            test_input_nonce_dec = {test_input_nonce_dec[7:0], test_input_nonce_dec[127:8]};
            #(PERIOD);
        end else if((nonce_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_nonce_dec[15:0]}, 2'b10);
            test_input_nonce_dec = {test_input_nonce_dec[15:0], test_input_nonce_dec[127:16]};
            #(PERIOD);
        end else if((nonce_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_nonce_dec[23:0]}, 2'b11);
            test_input_nonce_dec = {test_input_nonce_dec[23:0], test_input_nonce_dec[127:24]};
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
                test_input_ad_dec = {test_input_ad_dec[31:0], test_input_ad_dec[255:32]};
                #(PERIOD);
                test_iterator = test_iterator + 4;
            end
        end
        if((ad_size - test_iterator) == 1) begin
            test_duplex_simple_incomplete({24'b01, test_input_ad_dec[7:0]}, 2'b01);
            test_input_ad_dec = {test_input_ad_dec[7:0], test_input_ad_dec[255:8]};
            #(PERIOD);
        end else if((ad_size - test_iterator) == 2) begin
            test_duplex_simple_incomplete({16'b01,test_input_ad_dec[15:0]}, 2'b10);
            test_input_ad_dec = {test_input_ad_dec[15:0], test_input_ad_dec[255:16]};
            #(PERIOD);
        end else if((ad_size - test_iterator) == 3) begin
            test_duplex_simple_incomplete({8'b01,test_input_ad_dec[23:0]}, 2'b11);
            test_input_ad_dec = {test_input_ad_dec[23:0], test_input_ad_dec[255:24]};
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
                test_input_ct_dec = {test_input_ct_dec[31:0], test_input_ct_dec[255:32]};
                #(PERIOD);
                test_read_buffer(test_temp_buffer_out);
                test_output_pt_dec = {test_temp_buffer_out, test_output_pt_dec[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        if((pt_size - test_iterator) == 1) begin
            test_duplex_decrypt_incomplete({24'b01, test_input_ct_dec[7:0]}, 2'b01);
            test_input_ct_dec = {test_input_ct_dec[7:0], test_input_ct_dec[255:8]};
            #(PERIOD);
            test_read_buffer(test_temp_buffer_out);
            test_output_pt_dec = {test_temp_buffer_out[7:0], test_output_pt_dec[383:8]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 2) begin
            test_duplex_decrypt_incomplete({16'b01,test_input_ct_dec[15:0]}, 2'b10);
            test_input_ct_dec = {test_input_ct_dec[15:0], test_input_ct_dec[255:16]};
            #(PERIOD);
            test_read_buffer(test_temp_buffer_out);
            test_output_pt_dec = {test_temp_buffer_out[15:0], test_output_pt_dec[383:16]};
            #(PERIOD);
        end else if((pt_size - test_iterator) == 3) begin
            test_duplex_decrypt_incomplete({8'b01,test_input_ct_dec[23:0]}, 2'b11);
            test_input_ct_dec = {test_input_ct_dec[23:0], test_input_ct_dec[255:24]};
            #(PERIOD);
            test_read_buffer(test_temp_buffer_out);
            test_output_pt_dec = {test_temp_buffer_out[23:0], test_output_pt_dec[383:24]};
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
                test_read_buffer(test_temp_buffer_out);
                test_output_pt_dec = {test_temp_buffer_out, test_output_pt_dec[383:32]};
                test_iterator = test_iterator + 4;
                #(PERIOD);
            end
        end
        test_duplex_squeeze_simple();
        #(PERIOD);
        if((tag_size - test_iterator) == 1) begin
            test_read_buffer(test_temp_buffer_out);
            test_output_pt_dec = {test_temp_buffer_out[7:0], test_output_pt_dec[383:8]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 2) begin
            test_read_buffer(test_temp_buffer_out);
            test_output_pt_dec = {test_temp_buffer_out[15:0], test_output_pt_dec[383:16]};
            #(PERIOD);
        end else if((tag_size - test_iterator) == 3) begin
            test_read_buffer(test_temp_buffer_out);
            test_output_pt_dec = {test_temp_buffer_out[23:0], test_output_pt_dec[383:24]};
            #(PERIOD);
        end else begin
            test_read_buffer(test_temp_buffer_out);
            test_output_pt_dec = {test_temp_buffer_out[31:0], test_output_pt_dec[383:32]};
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
    $dumpvars(1, tb_subterranean_simple_axi4_lite_aead);
end

endmodule