`timescale 1ns / 1ps
`default_nettype none


//F and G
module drygascon128(
    input wire clk,
    input wire clk_en,
    input wire rst,
    input wire [31:0] din,
    input wire [3:0] ds,
    input wire wr_i,
    input wire wr_c,
    input wire wr_x,
    input wire [3:0] rounds,
    input wire start,
    input wire rd_r,
    input wire rd_c,
    output reg [31:0] dout,
    output reg idle
    );

`include "drygascon128_functions.v"

localparam C_QWORDS = 5;
localparam X_QWORDS = 2;
localparam R_QWORDS = 2;
localparam CIDX_WIDTH = 3;//log2(X_QWORDS)
localparam XIDX_WIDTH = 2;//log2(X_QWORDS*2)

localparam C_DWORDS = C_QWORDS * 2;
localparam C_WIDTH = C_QWORDS * 64;
localparam X_DWORDS = X_QWORDS * 2;
localparam X_WIDTH = X_QWORDS * 64;
localparam R_DWORDS = R_QWORDS*2;
localparam R_WIDTH = R_QWORDS*64;

reg [C_WIDTH-1:0] c;
reg [X_WIDTH-1:0] x;
reg [R_WIDTH-1:0] r;

reg absorb;
reg [3:0] cnt;

always @(posedge clk) begin
    if(clk_en) begin
        case(1'b1)
        rd_c: dout <= c[cnt*32+:32];
        rd_r: dout <= r[cnt*32+:32];
        default: dout <= {32{1'b0}};
        endcase
    end
end

localparam D_WIDTH = C_QWORDS*XIDX_WIDTH;
localparam MIX_ROUNDS = (R_WIDTH+4+D_WIDTH-1)/D_WIDTH;
localparam MIX_I_PAD = D_WIDTH*MIX_ROUNDS - R_WIDTH+4;
wire [D_WIDTH*MIX_ROUNDS-1:0] mix_i = {{MIX_I_PAD{1'b0}},ds,r};
wire [D_WIDTH-1:0] d = mix_i[cnt*D_WIDTH+:D_WIDTH];
wire [C_WIDTH-1:0] mixsx32_out = mixsx32(c,x,d);
wire [C_WIDTH-1:0] core_in = absorb ? mixsx32_out : c;
wire [3:0] core_round = absorb ? {4{1'b0}} : cnt;
wire [C_WIDTH-1:0] core_out = gascon5_round(core_in,core_round);
wire [128-1:0] accu_out = accumulate(core_out[0+:256],r);//free pipelining can be done here by taking c instead

localparam STATE_WIDTH = 2;
localparam STATE_IDLE = 2'b00;
localparam STATE_MIX_ENTRY = 2'b01;
localparam STATE_G_ENTRY = 2'b10;
reg [STATE_WIDTH-1:0] state;

always @(posedge clk) begin
    if(clk_en) begin
        if(rst) begin
            state <= STATE_IDLE;
            absorb <= 1'b0;
            cnt <= {4{1'b0}};
            idle <= 1'b1;
        end else begin
            case(state)
            STATE_IDLE: begin
                if(wr_i) begin
                    r[cnt*32+:32] <= din;
                    absorb <= 1'b1;
                end
                if(wr_c) begin
                    c[cnt*32+:32] <= din;
                end
                if(wr_x) begin
                    x <= {din,x[32+:X_WIDTH-32]};
                end
                case(1'b1)
                wr_c,rd_c: cnt <= (cnt + 1'b1) % C_DWORDS;
                wr_x:      cnt <= (cnt + 1'b1) % X_DWORDS;
                wr_i,rd_r: cnt <= (cnt + 1'b1) % R_DWORDS;
                endcase
                if(start) begin
                    if(absorb) begin
                        state <= STATE_MIX_ENTRY;
                    end else begin
                        r <= {R_WIDTH{1'b0}};
                        state <= STATE_G_ENTRY;
                    end
                    cnt <= {4{1'b0}};
                    idle <= 1'b0;
                end
            end
            STATE_MIX_ENTRY: begin
                c <= core_out;
                if(MIX_ROUNDS-2==cnt) begin
                    r <= {R_WIDTH{1'b0}};
                    cnt <= cnt +1'b1;//to get last chunk
                    state <= STATE_G_ENTRY;//let absorb for the first round to consume last chunk
                end else begin
                    cnt <= cnt +1'b1;
                end
            end
            STATE_G_ENTRY: begin
                //$display("round = %d",core_round);
                //$display("core_in:  %X",int_to_le(core_in));
                //$display("core_out: %X",int_to_le(core_out));
                //$display("accu_out: %X",int128_to_le(accu_out));
                absorb <= 1'b0;
                c <= core_out;
                r <= accu_out;
                if(rounds-1==cnt) begin
                    cnt <= {4{1'b0}};
                    state <= STATE_IDLE;
                    idle <= 1'b1;
                end else begin
                    cnt <= absorb ? {{3{1'b0}},1'b1} : cnt +1'b1;
                end
            end
            endcase
        end
    end
end

endmodule
`default_nettype wire
