//Configuration flags: "ACC_PIPE"

`timescale 1ns / 1ps
`default_nettype none


module birotr(
    input wire [64-1:0] din,
    input wire [ 6-1:0] shift,
    output reg [64-1:0] out
    );
wire [32-1:0] i0 = din[0*32+:32];
wire [32-1:0] i1 = din[1*32+:32];
wire [ 6-1:0] shift2 = shift>>1;
wire [ 6-1:0] shift3 = (shift2 + 1'b1) % 6'd32;
always @* begin
    if(shift & 1'b1) begin
        out[ 0+:32] = (i1>>shift2) | (i1 << (6'd32-shift2));
        out[32+:32] = (i0>>shift3) | (i0 << (6'd32-shift3));
    end else begin
        out[ 0+:32] = (i0>>shift2) | (i0 << (6'd32-shift2));
        out[32+:32] = (i1>>shift2) | (i1 << (6'd32-shift2));
    end
end
endmodule

module gascon5_round(
    input wire [320-1:0] din,
    input wire [4-1:0] round,
    output reg [320-1:0] out
    );
wire [5*6-1:0] rot_lut0 = {6'd07,6'd10,6'd01,6'd61,6'd19};
wire [5*6-1:0] rot_lut1 = {6'd40,6'd17,6'd06,6'd38,6'd28};
reg [7:0] round_constant;
always @* round_constant = (((4'hf - round)<<4) | round);
reg [320-1:0] add_constant_out;
always @* add_constant_out = din ^ {{5*64-8-2*64{1'b0}},round_constant,{2*64{1'b0}}};

reg [320-1:0] sbox_stage0;
always @* begin
    sbox_stage0 = add_constant_out;
    sbox_stage0[0*64+:64] =  add_constant_out[0*64+:64] ^ add_constant_out[4*64+:64];
    sbox_stage0[2*64+:64] =  add_constant_out[2*64+:64] ^ add_constant_out[1*64+:64];
    sbox_stage0[4*64+:64] =  add_constant_out[4*64+:64] ^ add_constant_out[3*64+:64];
end

reg [320-1:0] t;
always @* begin
    t[0*64+:64] =  (~sbox_stage0[0*64+:64]) & sbox_stage0[1*64+:64];
    t[1*64+:64] =  (~sbox_stage0[1*64+:64]) & sbox_stage0[2*64+:64];
    t[2*64+:64] =  (~sbox_stage0[2*64+:64]) & sbox_stage0[3*64+:64];
    t[3*64+:64] =  (~sbox_stage0[3*64+:64]) & sbox_stage0[4*64+:64];
    t[4*64+:64] =  (~sbox_stage0[4*64+:64]) & sbox_stage0[0*64+:64];
end

reg [320-1:0] sbox_stage1;
always @* begin
    sbox_stage1[0*64+:64] =  sbox_stage0[0*64+:64] ^ t[1*64+:64];
    sbox_stage1[1*64+:64] =  sbox_stage0[1*64+:64] ^ t[2*64+:64];
    sbox_stage1[2*64+:64] =  sbox_stage0[2*64+:64] ^ t[3*64+:64];
    sbox_stage1[3*64+:64] =  sbox_stage0[3*64+:64] ^ t[4*64+:64];
    sbox_stage1[4*64+:64] =  sbox_stage0[4*64+:64] ^ t[0*64+:64];
end

reg [320-1:0] sbox_stage2;
always @* begin
    sbox_stage2 = sbox_stage1;
    sbox_stage2[1*64+:64] =  sbox_stage1[1*64+:64] ^ sbox_stage1[0*64+:64];
    sbox_stage2[3*64+:64] =  sbox_stage1[3*64+:64] ^ sbox_stage1[2*64+:64];
    sbox_stage2[0*64+:64] =  sbox_stage1[0*64+:64] ^ sbox_stage1[4*64+:64];
end

reg [320-1:0] sbox_stage3;
always @* begin
    sbox_stage3 = sbox_stage2;
    sbox_stage3[2*64+:64] = ~sbox_stage2[2*64+:64];
end

wire [320-1:0] lin_layer_r0;
wire [320-1:0] lin_layer_r1;
birotr u_birotr00(.out(lin_layer_r0[0*64+:64]), .din(sbox_stage3[0*64+:64]), .shift(6'd19));
birotr u_birotr10(.out(lin_layer_r1[0*64+:64]), .din(sbox_stage3[0*64+:64]), .shift(6'd28));
birotr u_birotr01(.out(lin_layer_r0[1*64+:64]), .din(sbox_stage3[1*64+:64]), .shift(6'd61));
birotr u_birotr11(.out(lin_layer_r1[1*64+:64]), .din(sbox_stage3[1*64+:64]), .shift(6'd38));
birotr u_birotr02(.out(lin_layer_r0[2*64+:64]), .din(sbox_stage3[2*64+:64]), .shift(6'd01));
birotr u_birotr12(.out(lin_layer_r1[2*64+:64]), .din(sbox_stage3[2*64+:64]), .shift(6'd06));
birotr u_birotr03(.out(lin_layer_r0[3*64+:64]), .din(sbox_stage3[3*64+:64]), .shift(6'd10));
birotr u_birotr13(.out(lin_layer_r1[3*64+:64]), .din(sbox_stage3[3*64+:64]), .shift(6'd17));
birotr u_birotr04(.out(lin_layer_r0[4*64+:64]), .din(sbox_stage3[4*64+:64]), .shift(6'd07));
birotr u_birotr14(.out(lin_layer_r1[4*64+:64]), .din(sbox_stage3[4*64+:64]), .shift(6'd40));

reg [320-1:0] lin_layer;
always @* begin
    lin_layer[0*64+:64] =  sbox_stage3[0*64+:64] ^ lin_layer_r0[0*64+:64] ^ lin_layer_r1[0*64+:64];
    lin_layer[1*64+:64] =  sbox_stage3[1*64+:64] ^ lin_layer_r0[1*64+:64] ^ lin_layer_r1[1*64+:64];
    lin_layer[2*64+:64] =  sbox_stage3[2*64+:64] ^ lin_layer_r0[2*64+:64] ^ lin_layer_r1[2*64+:64];
    lin_layer[3*64+:64] =  sbox_stage3[3*64+:64] ^ lin_layer_r0[3*64+:64] ^ lin_layer_r1[3*64+:64];
    lin_layer[4*64+:64] =  sbox_stage3[4*64+:64] ^ lin_layer_r0[4*64+:64] ^ lin_layer_r1[4*64+:64];
end

always @* out = lin_layer;
endmodule


module mixsx32(
    input wire [320-1:0] c,
    input wire [128-1:0] x,
    input wire [10-1:0] d,
    output reg [320-1:0] out
    );
reg [2-1:0] idx0;
reg [32-1:0] xw0;
always @* idx0 = d[0*2+:2];
always @* xw0 = x[idx0*32+:32];
always @* out[0*64+:64] = c[0*64+:64] ^ {{32{1'b0}},xw0};
reg [2-1:0] idx1;
reg [32-1:0] xw1;
always @* idx1 = d[1*2+:2];
always @* xw1 = x[idx1*32+:32];
always @* out[1*64+:64] = c[1*64+:64] ^ {{32{1'b0}},xw1};
reg [2-1:0] idx2;
reg [32-1:0] xw2;
always @* idx2 = d[2*2+:2];
always @* xw2 = x[idx2*32+:32];
always @* out[2*64+:64] = c[2*64+:64] ^ {{32{1'b0}},xw2};
reg [2-1:0] idx3;
reg [32-1:0] xw3;
always @* idx3 = d[3*2+:2];
always @* xw3 = x[idx3*32+:32];
always @* out[3*64+:64] = c[3*64+:64] ^ {{32{1'b0}},xw3};
reg [2-1:0] idx4;
reg [32-1:0] xw4;
always @* idx4 = d[4*2+:2];
always @* xw4 = x[idx4*32+:32];
always @* out[4*64+:64] = c[4*64+:64] ^ {{32{1'b0}},xw4};
endmodule


module accumulate(
    input wire [256-1:0] din,
    input wire [128-1:0] r,
    output reg [128-1:0] out
    );
always @* out = r ^ din[0+:128] ^ {din[128+:32],din[128+32+:96]};
endmodule



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
reg [D_WIDTH-1:0] d;
wire [C_WIDTH-1:0] mixsx32_out;
reg [C_WIDTH-1:0] core_in;
reg [3:0] core_round;
wire [C_WIDTH-1:0] core_out;
wire [128-1:0] accu_out;

localparam MIX_I_PAD = D_WIDTH*MIX_ROUNDS - R_WIDTH+4;
reg [D_WIDTH*MIX_ROUNDS-1:0] mix_i;
always @* mix_i = {{MIX_I_PAD{1'b0}},ds,r};
always @* d = mix_i[cnt*D_WIDTH+:D_WIDTH];
mixsx32 u_mixsx32(.out(mixsx32_out), .c(c), .x(x), .d(d));
always @* core_in = absorb ? mixsx32_out : c;
always @* core_round = absorb ? {4{1'b0}} : cnt;
gascon5_round u_gascon5_round(.out(core_out), .din(core_in), .round(core_round));
accumulate u_accumulate(.out(accu_out), .din(c[0+:256]), .r(r));

localparam STATE_WIDTH = 2;
localparam STATE_IDLE = 2'b00;
localparam STATE_MIX_ENTRY = 2'b01;
localparam STATE_G_ENTRY = 2'b10;
localparam STATE_G_EXIT = 2'b11;
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
                if(!absorb && (cnt >= 1)) r <= accu_out;
                if(rounds-1==cnt) begin
                    cnt <= {4{1'b0}};
                    state <= STATE_G_EXIT;
                end else begin
                    cnt <= absorb ? {{3{1'b0}},1'b1} : cnt +1'b1;
                end
            end
            STATE_G_EXIT: begin
                r <= accu_out;
                state <= STATE_IDLE;
                idle <= 1'b1;
            end
            default: begin
            end
            endcase
        end
    end
end

endmodule
`default_nettype wire
