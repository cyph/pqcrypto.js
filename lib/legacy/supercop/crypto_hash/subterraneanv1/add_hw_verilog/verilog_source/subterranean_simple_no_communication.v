module subterranean_simple_no_communication
(
    input clk,
    input arstn,
    input start_operation,
    input [3:0] operation_type,
    input [31:0] buffer_in,
    input [1:0] buffer_in_size,
    output [31:0] buffer_out,
    output core_free,
    output core_finish
);

wire [256:0] permutation_a;
reg [32:0] permutation_i;
wire [256:0] permutation_o;

reg [31:0] buffer_in_mask_for_buffer_out;
wire [31:0] state_with_buffer_in_for_buffer_out;
reg control_bit;

reg [256:0] permutation_state;
wire [31:0] permutation_state_out;
reg [31:0] permutation_state_out_mask_decryption;
reg [31:0] reg_buffer_out;


reg cipher_internal_core_free;
reg cipher_internal_finish;


always @(*) begin
    case(operation_type)
        4'h1 : begin // Duplex Simple
            control_bit <= 1'b1;
        end
        4'h2 : begin // Duplex Simple Incomplete
            control_bit <= 1'b0;
        end
        4'h3 : begin // Duplex Encrypt
            control_bit <= 1'b1;
        end
        4'h4 : begin // Duplex Encrypt Incomplete
            control_bit <= 1'b0;
        end
        4'h5 : begin // Duplex Decrypt
            control_bit <= 1'b1;
        end
        4'h6 : begin // Duplex Decrypt Incomplete
            control_bit <= 1'b0;
        end
        default : begin
            control_bit <= 1'b0;
        end
    endcase
end

always @(*) begin
    permutation_i[32] <= control_bit;
    case(operation_type)
        4'h5 : begin // Duplex Decrypt
            permutation_i[31:0] <= state_with_buffer_in_for_buffer_out;
        end
        4'h6 : begin // Duplex Decrypt Incomplete
            permutation_i[31:0] <= state_with_buffer_in_for_buffer_out;
        end
        default : begin
            permutation_i[31:0] <= buffer_in;
        end
    endcase
end

assign permutation_a = permutation_state;

subterranean_round
permutation (
    .a(permutation_a),
    .i(permutation_i),
    .o(permutation_o)
);

always @(*) begin
    if(operation_type == 4'h7) begin
        buffer_in_mask_for_buffer_out <= 32'h00000000;
    end else begin
        buffer_in_mask_for_buffer_out <= 32'hFFFFFFFF;
    end
end

always @(*) begin
    case(operation_type)
        4'h6 : begin // Duplex Decrypt Incomplete
            if(buffer_in_size == 2'b00) begin
                permutation_state_out_mask_decryption <= 32'h00000000;
            end else if(buffer_in_size == 2'b01) begin
                permutation_state_out_mask_decryption <= 32'h000000FF;
            end else if(buffer_in_size == 2'b10) begin
                permutation_state_out_mask_decryption <= 32'h0000FFFF;
            end else begin
                permutation_state_out_mask_decryption <= 32'h00FFFFFF;
            end
        end
        default : begin
            permutation_state_out_mask_decryption <= 32'hFFFFFFFF;
        end
    endcase
end

assign permutation_state_out[0] = (permutation_state[1] ^ permutation_state[256])    & permutation_state_out_mask_decryption[0];
assign permutation_state_out[1] = (permutation_state[176] ^ permutation_state[81])   & permutation_state_out_mask_decryption[1];
assign permutation_state_out[2] = (permutation_state[136] ^ permutation_state[121])  & permutation_state_out_mask_decryption[2];
assign permutation_state_out[3] = (permutation_state[35] ^ permutation_state[222])   & permutation_state_out_mask_decryption[3];
assign permutation_state_out[4] = (permutation_state[249] ^ permutation_state[8])    & permutation_state_out_mask_decryption[4];
assign permutation_state_out[5] = (permutation_state[134] ^ permutation_state[123])  & permutation_state_out_mask_decryption[5];
assign permutation_state_out[6] = (permutation_state[197] ^ permutation_state[60])   & permutation_state_out_mask_decryption[6];
assign permutation_state_out[7] = (permutation_state[234] ^ permutation_state[23])   & permutation_state_out_mask_decryption[7];
assign permutation_state_out[8] = (permutation_state[64] ^ permutation_state[193])   & permutation_state_out_mask_decryption[8];
assign permutation_state_out[9] = (permutation_state[213] ^ permutation_state[44])   & permutation_state_out_mask_decryption[9];
assign permutation_state_out[10] = (permutation_state[223] ^ permutation_state[34])  & permutation_state_out_mask_decryption[10];
assign permutation_state_out[11] = (permutation_state[184] ^ permutation_state[73])  & permutation_state_out_mask_decryption[11];
assign permutation_state_out[12] = (permutation_state[2] ^ permutation_state[255])   & permutation_state_out_mask_decryption[12];
assign permutation_state_out[13] = (permutation_state[95] ^ permutation_state[162])  & permutation_state_out_mask_decryption[13];
assign permutation_state_out[14] = (permutation_state[15] ^ permutation_state[242])  & permutation_state_out_mask_decryption[14];
assign permutation_state_out[15] = (permutation_state[70] ^ permutation_state[187])  & permutation_state_out_mask_decryption[15];
assign permutation_state_out[16] = (permutation_state[241] ^ permutation_state[16])  & permutation_state_out_mask_decryption[16];
assign permutation_state_out[17] = (permutation_state[11] ^ permutation_state[246])  & permutation_state_out_mask_decryption[17];
assign permutation_state_out[18] = (permutation_state[137] ^ permutation_state[120]) & permutation_state_out_mask_decryption[18];
assign permutation_state_out[19] = (permutation_state[211] ^ permutation_state[46])  & permutation_state_out_mask_decryption[19];
assign permutation_state_out[20] = (permutation_state[128] ^ permutation_state[129]) & permutation_state_out_mask_decryption[20];
assign permutation_state_out[21] = (permutation_state[169] ^ permutation_state[88])  & permutation_state_out_mask_decryption[21];
assign permutation_state_out[22] = (permutation_state[189] ^ permutation_state[68])  & permutation_state_out_mask_decryption[22];
assign permutation_state_out[23] = (permutation_state[111] ^ permutation_state[146]) & permutation_state_out_mask_decryption[23];
assign permutation_state_out[24] = (permutation_state[4] ^ permutation_state[253])   & permutation_state_out_mask_decryption[24];
assign permutation_state_out[25] = (permutation_state[190] ^ permutation_state[67])  & permutation_state_out_mask_decryption[25];
assign permutation_state_out[26] = (permutation_state[30] ^ permutation_state[227])  & permutation_state_out_mask_decryption[26];
assign permutation_state_out[27] = (permutation_state[140] ^ permutation_state[117]) & permutation_state_out_mask_decryption[27];
assign permutation_state_out[28] = (permutation_state[225] ^ permutation_state[32])  & permutation_state_out_mask_decryption[28];
assign permutation_state_out[29] = (permutation_state[22] ^ permutation_state[235])  & permutation_state_out_mask_decryption[29];
assign permutation_state_out[30] = (permutation_state[17] ^ permutation_state[240])  & permutation_state_out_mask_decryption[30];
assign permutation_state_out[31] = (permutation_state[165] ^ permutation_state[92])  & permutation_state_out_mask_decryption[31];

assign state_with_buffer_in_for_buffer_out = permutation_state_out ^ (buffer_in & buffer_in_mask_for_buffer_out);

always @(posedge clk) begin
    if (start_operation == 1'b1) begin
        case(operation_type)
            4'h0 : begin // Initialize
                permutation_state <= 256'h0;
            end
            4'h1 : begin // Duplex Simple
                permutation_state <= permutation_o;
                reg_buffer_out <= state_with_buffer_in_for_buffer_out;
            end
            4'h2 : begin // Duplex Simple Incomplete
                permutation_state <= permutation_o;
                reg_buffer_out <= state_with_buffer_in_for_buffer_out;
            end
            4'h3 : begin // Duplex Encrypt
                permutation_state <= permutation_o;
                reg_buffer_out <= state_with_buffer_in_for_buffer_out;
            end
            4'h4 : begin // Duplex Encrypt Incomplete
                permutation_state <= permutation_o;
                reg_buffer_out <= state_with_buffer_in_for_buffer_out;
            end
            4'h5 : begin // Duplex Decrypt
                permutation_state <= permutation_o;
                reg_buffer_out <= state_with_buffer_in_for_buffer_out;
            end
            4'h6 : begin // Duplex Decrypt Incomplete
                permutation_state <= permutation_o;
                reg_buffer_out <= state_with_buffer_in_for_buffer_out;
            end
            4'h7 : begin // Duplex Squeeze
                permutation_state <= permutation_o;
                reg_buffer_out <= state_with_buffer_in_for_buffer_out;
            end
            4'h8 : begin // Read Buffer
            end
        endcase
    end
end

assign buffer_out = reg_buffer_out;

always @(posedge clk or negedge arstn) begin
    if (!arstn) begin
        cipher_internal_core_free <= 1'b0;
        cipher_internal_finish <= 1'b0;
    end else begin
        if(cipher_internal_core_free == 1'b1) begin
            if(start_operation == 1'b1) begin
                cipher_internal_core_free <= 1'b0;
            end
            cipher_internal_finish <= 1'b0;
        end else begin
            cipher_internal_core_free <= 1'b1;
            cipher_internal_finish <= 1'b1;
        end
    end
end

assign core_free = cipher_internal_core_free;
assign core_finish = cipher_internal_finish;

endmodule