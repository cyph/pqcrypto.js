`timescale 1ns / 1ps
`default_nettype none
`define assert(signal, value) \
        if (signal !== value) begin \
            $display("%t ASSERTION FAILED in %m: %x != %x",$realtime,signal,value); \
            #100; \
            $finish; \
        end

module tb_drygascon128();

//`define func_path u_dut

`ifndef func_path
`define func_path tb_drygascon128
`include "drygascon128_functions.v"
`endif

reg clk;
reg clk_en;
reg reset;
reg [31:0] din;
reg [3:0] ds;
reg wr_i;
reg wr_c;
reg wr_x;
reg [3:0] rounds;
reg start;
reg rd_r;
reg rd_c;
wire [31:0] dout;
wire idle;

drygascon128 u_dut(
    .clk(clk),
    .clk_en(clk_en),
    .rst(reset),
    .din(din),
    .ds(ds),
    .wr_i(wr_i),
    .wr_c(wr_c),
    .wr_x(wr_x),
    .rounds(rounds),
    .start(start),
    .rd_r(rd_r),
    .rd_c(rd_c),
    .dout(dout),
    .idle(idle)
);

wire [63:0] c0 = u_dut.c[0*64+:64];
wire [63:0] c1 = u_dut.c[1*64+:64];
wire [63:0] c2 = u_dut.c[2*64+:64];
wire [63:0] c3 = u_dut.c[3*64+:64];
wire [63:0] c4 = u_dut.c[4*64+:64];

reg [320-1:0] c_out;
reg [128-1:0] r_out;

wire done = idle;


task wait_clock;
begin
    @ (negedge clk);
end
endtask

task reset_dut;
begin
    wait_clock();
    reset = 1;
    wait_clock();
    reset = 0;
end
endtask

task wait_clocks;
    input integer nclocks;
    integer i;
begin
    for(i=0;i < nclocks; i= i+1) begin
        wait_clock();
    end
end
endtask

task start_op;
begin
    start = 1;
    @(negedge clk);
    start = 0;
end
endtask

reg done_reg;
always @(posedge clk) done_reg <= done;

task wait_done;
begin
    wait_clock();
    while(!done) begin
        wait_clock();
    end
end
endtask

task write_x;
    input [128-1:0] x;
    integer i;
begin
    wr_x = 1;
    for(i=0;i<4;i=i+1) begin
        din = x[i*32+:32];
        wait_clock();
    end
    wr_x=0;
end
endtask

task write_x_le;
    input [128-1:0] x;
begin
    write_x(`func_path.le_to_int128(x));
end
endtask

task write_c;
    input [320-1:0] c;
    integer i;
begin
    wr_c = 1;
    for(i=0;i<10;i=i+1) begin
        din = c[i*32+:32];
        wait_clock();
    end
    wr_c=0;
end
endtask

task write_c_le;
    input [320-1:0] c;
begin
    write_c(`func_path.le_to_int(c));
end
endtask

task read_c;
    integer i;
begin
    rd_c = 1;
    for(i=0;i<10;i=i+1) begin
        wait_clock();
        c_out[i*32+:32] = dout;
    end
    rd_c=0;
end
endtask

task check_c;
    input [320-1:0] expected;
begin
    read_c();
    `assert(c_out,expected)
end
endtask

task check_c_le;
    input [320-1:0] expected;
begin
    check_c(`func_path.le_to_int(expected));
end
endtask

task write_i;
    input [128-1:0] di;
    integer i;
begin
    wr_i = 1;
    for(i=0;i<4;i=i+1) begin
        din = di[i*32+:32];
        wait_clock();
    end
    wr_i=0;
end
endtask

task write_i_le;
    input [128-1:0] i;
begin
    write_i(`func_path.le_to_int128(i));
end
endtask


task read_r;
    integer i;
begin
    rd_r = 1;
    for(i=0;i<4;i=i+1) begin
        wait_clock();
        r_out[i*32+:32] = dout;
    end
    rd_r=0;
end
endtask

task check_r;
    input [128-1:0] expected;
begin
    read_r();
    `assert(r_out,expected)
end
endtask

task check_r_le;
    input [128-1:0] expected;
begin
    check_r(`func_path.le_to_int128(expected));
end
endtask

task gascon128_f;
    input [3:0] ds_in;
    input [3:0] rounds_in;
    input [128-1:0] i;
begin
rounds=rounds_in;
ds = ds_in;
write_i(i);
start_op();
wait_done();
end
endtask

task gascon128_g;
    input [3:0] rounds_in;
begin
rounds=rounds_in;
start_op();
wait_done();
end
endtask

task gascon128_f_le;
    input [3:0] ds_in;
    input [3:0] rounds_in;
    input [128-1:0] i;
begin
gascon128_f(ds_in,rounds_in,`func_path.le_to_int128(i));
end
endtask

task check_gascon128_f_le;
    input [3:0] ds_in;
    input [3:0] rounds_in;
    input [128-1:0] i;
    input [128-1:0] expected;
begin
gascon128_f_le(ds_in,rounds_in,i);
check_r_le(expected);
end
endtask

task check_gascon128_g_le;
    input [3:0] rounds_in;
    input [128-1:0] expected;
begin
gascon128_g(rounds_in);
check_r_le(expected);
end
endtask

initial begin
clk = 0;
forever #5 clk = ~clk;
end

`define DRYGASCON128_IROUNDS 11
`define DRYGASCON128_ROUNDS   7

task init_dut;
begin
start = 0;
clk_en = 1;
wr_c=0;
wr_x=0;
wr_i=0;
rd_c=0;
rd_r=0;
ds=0;
rounds=11;
reset_dut();
end
endtask

task basic_test;
begin
    init_dut();
    write_x_le(128'h28292A2B2C2D2E2F3031323334353637);
    `assert(u_dut.x,`func_path.le_to_int128(128'h28292A2B2C2D2E2F3031323334353637))
    write_c_le(320'h000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627);
    `assert(u_dut.c,`func_path.le_to_int(320'h000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627))
    check_c_le(320'h000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627);
    check_gascon128_f_le(6,`DRYGASCON128_IROUNDS,128'hF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF,128'hF1FBA3D719B00A49BF170F832EB7649F);
    write_c_le(320'h000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627);
    check_gascon128_f_le(6,`DRYGASCON128_IROUNDS,128'hF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF,128'hF1FBA3D719B00A49BF170F832EB7649F);
end
endtask

`define PAD 1
`define FINAL 1
`define DS_S 2
`define DS_D 1
`define DS_A 2
`define DS_M 3

task init_hash;
begin
    init_dut();
    write_x_le(128'hA4093822299F31D0082EFA98EC4E6C89);
    `assert(u_dut.x,`func_path.le_to_int128(128'hA4093822299F31D0082EFA98EC4E6C89))
    write_c_le(320'h243F6A8885A308D313198A2E03707344243F6A8885A308D313198A2E03707344243F6A8885A308D3);
    `assert(u_dut.c,`func_path.le_to_int(320'h243F6A8885A308D313198A2E03707344243F6A8885A308D313198A2E03707344243F6A8885A308D3))
    check_c_le(320'h243F6A8885A308D313198A2E03707344243F6A8885A308D313198A2E03707344243F6A8885A308D3);
end
endtask

task hash_null;
begin
    init_hash();
    check_gascon128_f_le(`func_path.compute_ds(`PAD,`DS_S,`FINAL),`DRYGASCON128_ROUNDS,128'h01000000000000000000000000000000,128'h1EDC77386E20A37C721D6E77ADABB9C4);
    check_gascon128_g_le(`DRYGASCON128_ROUNDS,128'h830F199F5ED25284A13C1D84B9FC257A);
end
endtask

task hash_detailed_tv;
begin
    init_hash();
    check_gascon128_f_le(`func_path.compute_ds(`PAD,`DS_S,`FINAL),`DRYGASCON128_ROUNDS,128'h00010203040506070100000000000000,128'hCDE2DEE0235345CBFA51EC2CE5743571);
    check_gascon128_g_le(`DRYGASCON128_ROUNDS,128'h8EC0133EC2756E035FA404C1CE511E24);
end
endtask

task hash_tv17;
begin
    init_hash();
    gascon128_f_le(0,`DRYGASCON128_ROUNDS,128'h000102030405060708090A0B0C0D0E0F);
    check_gascon128_f_le(`func_path.compute_ds(`PAD,`DS_S,`FINAL),`DRYGASCON128_ROUNDS,128'h10010000000000000000000000000000,128'h20CDB78974D692100612978096CCFE82);
    check_gascon128_g_le(`DRYGASCON128_ROUNDS,128'hE39F15969F493FAD8FA870F93B7252EA);
end
endtask

initial begin
    basic_test();
    hash_null();
    hash_detailed_tv();
    hash_tv17();
    $display("sim pass");
    $finish();
end

endmodule
`default_nettype wire
