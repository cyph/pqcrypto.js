`ifndef __DRYGASCON128_FUNCTIONS__
`define __DRYGASCON128_FUNCTIONS__

function [320-1:0] int_to_le;
    input [320-1:0] din;
    reg [320-1:0] o;
    integer i;
begin
    o = {320{1'b0}};
    for(i=0;i<8*5;i=i+1) begin
        o[320-1-i*8-:8] = din[i*8+:8];
    end
    int_to_le = o;
end
endfunction

function [320-1:0] le_to_int;
    input [320-1:0] din;
    reg [320-1:0] o;
    integer i;
begin
    o = {320{1'b0}};
    for(i=0;i<8*5;i=i+1) begin
        o[i*8+:8] = din[320-1-i*8-:8];
    end
    le_to_int = o;
end
endfunction

function [128-1:0] int128_to_le;
    input [128-1:0] din;
    reg [128-1:0] o;
    integer i;
begin
    o = {128{1'b0}};
    for(i=0;i<8*4;i=i+1) begin
        o[128-1-i*8-:8] = din[i*8+:8];
    end
    int128_to_le = o;
end
endfunction

function [128-1:0] le_to_int128;
    input [128-1:0] din;
    reg [128-1:0] o;
    integer i;
begin
    o = {128{1'b0}};
    for(i=0;i<8*4;i=i+1) begin
        o[i*8+:8] = din[128-1-i*8-:8];
    end
    le_to_int128 = o;
end
endfunction

function [64-1:0] int64_to_le;
    input [64-1:0] din;
    reg [64-1:0] o;
    integer i;
begin
    o = {64{1'b0}};
    for(i=0;i<8;i=i+1) begin
        o[64-1-i*8-:8] = din[i*8+:8];
    end
    int64_to_le = o;
end
endfunction

function [64-1:0] le_to_int64;
    input [64-1:0] din;
    reg [64-1:0] o;
    integer i;
begin
    o = {64{1'b0}};
    for(i=0;i<8;i=i+1) begin
        o[i*8+:8] = din[64-1-i*8-:8];
    end
    le_to_int64 = o;
end
endfunction

function [128-1:0] accumulate;
    input [256-1:0] din;
    input [128-1:0] r;
    reg [128-1:0] o;
begin
    accumulate = r ^ din[0+:128] ^ {din[128+:32],din[128+32+:96]};
end
endfunction

function [32-1:0] rotr32;
    input [32-1:0] din;
    input [ 5-1:0] shift;
    reg [5:0] i;
    reg [32-1:0] o;
begin
    o = {32{1'b0}};
    for(i=0;i<32;i=i+1'b1) begin
        //o[(i+32-shift)%32] = din[i];
        o[i] = din[(i+shift)%32];
    end
    rotr32 = o;
end
endfunction

function [64-1:0] birotr;
    input [64-1:0] din;
    input [ 6-1:0] shift;
    reg [32-1:0] i0;
    reg [32-1:0] i1;
    reg [32-1:0] t;
    integer shift2;
begin
    shift2 = shift>>1;
    i0 = din[0*32+:32];
    i1 = din[1*32+:32];
    if(shift & 1'b1) begin
        t = rotr32(i1,shift2);
        i1 = rotr32(i0,(shift2+1'b1)%32);
        i0 = t;
    end else begin
        i0 = rotr32(i0,shift2);
        i1 = rotr32(i1,shift2);
    end
    birotr = {i1,i0};
end
endfunction

wire [5*6-1:0] rot_lut0 = {6'd07,6'd10,6'd01,6'd61,6'd19};
wire [5*6-1:0] rot_lut1 = {6'd40,6'd17,6'd06,6'd38,6'd28};

//function print_words64;
//    input [320-1:0] c;
//    integer i;
//begin
//    for(i=0;i<5;i=i+1) begin
//        $display("%x",c[i*64+:64]);
//    end
//    print_words64 = 0;
//end
//endfunction

function [320-1:0] gascon5_round;
    input [320-1:0] din;
    input [4-1:0] round;
    reg [4-1:0] r;
    integer i;
    integer mid;
    integer cwords;
    integer d;
    integer s;
    reg [64-1:0] wi;
    reg [64-1:0] ws;
    reg [320-1:0] t;
    reg [320-1:0] c;
    integer dummy;
begin
    c = din;
    mid = 2;
    cwords = 5;
    //$display("i: %x",c);
    //add constant
    r = round;
    c[mid*64+:64] = c[mid*64+:64] ^ (((4'hf - r)<<4) | r);
    //$display("c mid: %x, r=%x",c[mid*64+:64],r);
    //$display("c: %x",c);
    //sbox
    for(i=0;i<=2;i=i+1'b1) begin
        d = 2*i;
        s = (cwords + d - 1'b1) % cwords;
        c[d*64+:64] = c[d*64+:64] ^ c[s*64+:64];
    end

    for(i=0;i<cwords;i=i+1'b1) begin
        s = (i+1) % cwords;
        wi = c[i*64+:64];
        ws = c[s*64+:64];
        t[i*64+:64] = (~wi) & ws;
    end

    for(i=0;i<cwords;i=i+1'b1) begin
        s = (i+1) % cwords;
        c[i*64+:64] = c[i*64+:64] ^ t[s*64+:64];
    end

    for(i=0;i<=2;i=i+1'b1) begin
        s = 2*i;
        d = (s + 1'b1) % cwords;
        c[d*64+:64] = c[d*64+:64] ^ c[s*64+:64];
    end
    c[mid*64+:64] = ~c[mid*64+:64];
    //$display("c: %x",c);

    //linlayer
    for(i=0;i<3'd5;i=i+1'b1) begin
        c[i*64+:64] = c[i*64+:64] ^ birotr(c[i*64+:64],rot_lut0[i*6+:6]) ^ birotr(c[i*64+:64],rot_lut1[i*6+:6]);
    end

    //$display("c: %x",c);
    //$display("c: ");dummy=print_words64(c);
    gascon5_round = c;
end
endfunction

function [320-1:0] mixsx32;
    input [320-1:0] c;
    input [128-1:0] x;
    input [10-1:0] d;
    reg [320-1:0] co;
    reg [2-1:0] idx;
    reg [32-1:0] xw;
    integer j;
begin
    co = c;
    for(j=0;j<5;j=j+1) begin
        idx = d[j*2+:2];
        xw = x[idx*32+:32];
        co[j*2*32+:32] = co[j*2*32+:32] ^ xw;
    end
    mixsx32 = co;
end
endfunction

function [3:0] compute_ds;
    input padded;
    input [1:0] domain;
    input finalize;
begin
    compute_ds = {domain,finalize,padded};
end
endfunction

`endif
