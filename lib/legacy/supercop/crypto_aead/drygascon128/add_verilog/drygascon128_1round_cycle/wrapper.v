//wrapper module to reduce pin count and also for synthesis trial without taking care of I/O delays
`default_nettype none
module wrapper(
    input wire clk,
    input wire tc,//test capture: update ios reg
    input wire ts,//test shift, data shift in from di and shift out to do
    input wire di,
    output reg do //output idle when ts=0
);
localparam INPUT_WIDTH = 1+32+4+3+4+3;
localparam OUTPUT_WIDTH = 32+1;
localparam IOS_WIDTH = OUTPUT_WIDTH+INPUT_WIDTH;

wire io_mode = tc | ts;

reg clk_en;
always @(posedge clk) clk_en <= ~io_mode;

reg rst;
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

drygascon128 u_impl(
    .clk(clk),
    .clk_en(clk_en),
    .rst(rst),
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

reg [IOS_WIDTH-1:0] ios;

always @(*) begin
    {rst,din,ds,wr_i,wr_c,wr_x,rounds,start,rd_r,rd_c} = ios[0+:INPUT_WIDTH];
end

always @(posedge clk) do <= io_mode ? ios[IOS_WIDTH-1] : idle;
wire [OUTPUT_WIDTH-1:0] captured_output = ios[INPUT_WIDTH+:OUTPUT_WIDTH];

always @(posedge clk) begin
    if(io_mode) begin
        if(tc) ios[INPUT_WIDTH+:OUTPUT_WIDTH] <= {idle,dout};
        else if(ts) ios <= {ios[0+:IOS_WIDTH-1],di};
    end
end
endmodule
`default_nettype wire
