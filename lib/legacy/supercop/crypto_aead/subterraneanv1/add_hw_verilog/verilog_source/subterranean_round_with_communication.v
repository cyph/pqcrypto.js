module subterranean_round_with_communication
(
    input clk,
    input arstn,
    input start,
    input data_in_valid,
    input data_out_ready,
    input data_in,
    output data_out,
    output data_out_valid,
    output data_in_ready,
    output finish,
    output core_free
);

wire [256:0] permutation_a;
wire [32:0] permutation_i;
wire [256:0] permutation_o;

reg internal_start;
reg internal_data_in_ready;
reg internal_data_out_valid;
reg internal_finish;

reg [256:0] input_output_buffer;

subterranean_round
permutation (
    .a(permutation_a),
    .i(permutation_i),
    .o(permutation_o)
);

always @(posedge clk or negedge arstn) begin
    if (!arstn) begin
        internal_start <= 1'b0;
        internal_finish <= 1'b0;
    end else begin
        internal_start <= start;
        internal_finish <= internal_start;
    end
end

always @(posedge clk) begin
    if (internal_start) begin
        input_output_buffer <= permutation_o;
    end else if (data_in_valid) begin
        input_output_buffer <= {data_in, input_output_buffer[256:1]};
    end else if (data_out_ready == 1'b1 && internal_data_out_valid == 1'b1) begin
        input_output_buffer <= {input_output_buffer[0], input_output_buffer[256:1]};
    end
end

always @(posedge clk or negedge arstn) begin
    if (!arstn) begin
        internal_data_out_valid <= 1'b0;
    end else begin
        if(internal_start) begin
            internal_data_out_valid <= 1'b0;
        end else begin
            if(data_out_ready) begin
                internal_data_out_valid <= 1'b1;
            end else begin
                internal_data_out_valid <= 1'b0;
            end
        end
    end
end

always @(posedge clk or negedge arstn) begin
    if (!arstn) begin
        internal_data_in_ready <= 1'b1;
    end else begin
        if(start) begin
            internal_data_in_ready <= 1'b0;
        end else if(internal_start) begin
            internal_data_in_ready <= 1'b1;
        end
    end
end

assign permutation_a  = input_output_buffer;
assign permutation_i  = 33'b0;

assign data_in_ready = internal_data_in_ready;
assign data_out = input_output_buffer[0];
assign data_out_valid = internal_data_out_valid;
assign finish = internal_finish;
assign core_free = ~internal_start;

endmodule