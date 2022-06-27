`timescale 1ns / 1ps
module tb_subterranean_round
#(parameter PERIOD = 1000,
maximum_number_of_tests = 100,
test_memory_file_subterranean_permutation = "../data_tests/subterranean_permutation.dat"
);

reg [256:0] test_state;
reg [32:0] test_input;
wire [256:0] test_new_state;
reg [256:0] true_new_state;


reg clk;
reg test_error = 1'b0;
reg test_verification = 1'b0;

localparam tb_delay = PERIOD/2;
localparam tb_delay_read = 3*PERIOD/4;

subterranean_round
test (
    .a(test_state),
    .i(test_input),
    .o(test_new_state)
);
    
initial begin : clock_generator
    clk <= 1'b1;
    forever begin
        #(PERIOD/2);
        clk <= ~clk;
    end
end

integer ram_file;
integer number_of_tests;
integer test_iterator;
integer status_ram_file;
initial begin
    test_state <= 257'b0;
    test_input <= 33'b0;
    #(PERIOD*2);
    #(tb_delay);
    ram_file = $fopen(test_memory_file_subterranean_permutation, "r");
    status_ram_file = $fscanf(ram_file, "%d", number_of_tests);
    #(PERIOD);
    if((number_of_tests > maximum_number_of_tests) && (maximum_number_of_tests != 0)) begin
        number_of_tests = maximum_number_of_tests;
    end
    for (test_iterator = 1; test_iterator < number_of_tests; test_iterator = test_iterator + 1) begin
        test_error <= 1'b0;
        test_verification <= 1'b0;
        status_ram_file = $fscanf(ram_file, "%b", test_state);
        status_ram_file = $fscanf(ram_file, "%b", true_new_state);
        #PERIOD;
        test_verification <= 1'b1;
        if (true_new_state == test_new_state) begin
            test_error <= 1'b0;
        end else begin
            test_error <= 1'b1;
            $display("Computed values do not match expected ones");
        end
        #PERIOD;
        test_error <= 1'b0;
        test_verification <= 1'b0;
        #PERIOD;
    end
    $fclose(ram_file);
    $display("End of the test.");
    disable clock_generator;
    #(PERIOD);
end

initial
begin
    $dumpfile("dump.vcd");
    $dumpvars(0, tb_subterranean_round);
end

endmodule