`timescale 1ns / 1ps
module tb_subterranean_simple_axi4_lite_hash
#(parameter PERIOD = 1000,
maximum_line_length = 10000,
test_memory_file_subterranean_hash = "../data_tests/LWC_HASH_KAT_256.txt"
);

reg [8191:0] test_input_hash;
reg [255:0] test_output_hash;
reg [255:0] true_output_hash;

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
        axi_4_lite_write_value({4'h2, sigma_size,2'b00}, sigma, 2'b11);
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
        axi_4_lite_write_value({4'h4, sigma_size,2'b00}, sigma, 2'b11);
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
integer message_size;
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
    test_input_hash <= 8192'b0;
    test_output_hash <= 256'b0;
    true_output_hash <= 256'b0;
    #(PERIOD*2);
    test_aresetn <= 1'b1;
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
        #(PERIOD);
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
        #(PERIOD);
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
            test_read_buffer(test_temp_buffer_out);
            test_output_hash <= {test_temp_buffer_out, test_output_hash[255:32]};
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
    $dumpvars(1, tb_subterranean_simple_axi4_lite_hash);
 end


endmodule