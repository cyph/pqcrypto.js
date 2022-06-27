module subterranean_simple_axi4_lite
(
    input aclk,
    input aresetn,
    // Write Address related signals
    input [7:0] s_axi_awaddr,
    input [2:0] s_axi_awprot,
    input s_axi_awvalid,
    output s_axi_awready,
    // Write data signals
    input [31:0] s_axi_wdata,
    input [3:0] s_axi_wstrb,
    input s_axi_wvalid,
    output s_axi_wready,
    // Response channel
    output [1:0] s_axi_bresp,
    output s_axi_bvalid,
    input s_axi_bready,
    // Read Address related signals
    input [7:0] s_axi_araddr,
    input [2:0] s_axi_arprot,
    input s_axi_arvalid,
    output s_axi_arready,
    // Read data signals
    output [31:0] s_axi_rdata,
    output [1:0] s_axi_rresp,
    output s_axi_rvalid,
    input s_axi_rready
);


reg [7:0] reg_s_axi_awaddr;
reg [2:0] reg_s_axi_awprot;
reg reg_s_axi_awvalid;
reg reg_s_axi_awready;

reg reg_s_axi_awaddr_is_good;

reg [31:0] reg_s_axi_wdata;
reg [3:0] reg_s_axi_wstrb;
reg reg_s_axi_wvalid;
reg reg_s_axi_wready;

wire [1:0] reg_s_axi_wdata_number_bytes;

reg [1:0] reg_s_axi_bresp;
reg reg_s_axi_bvalid;

reg [7:0] reg_s_axi_araddr;
reg [2:0] reg_s_axi_arprot;
reg reg_s_axi_arvalid;
reg reg_s_axi_arready;

reg reg_s_axi_araddr_is_good;

reg [31:0] reg_s_axi_rdata;
reg [1:0] reg_s_axi_rresp;
reg reg_s_axi_rvalid;

reg cipher_start_operation;
wire [3:0] cipher_operation_type;
wire [31:0] cipher_buffer_in;
wire [1:0] cipher_buffer_in_size;
wire [31:0] cipher_buffer_out;
wire cipher_core_free;
wire cipher_core_finish;




assign s_axi_awready = reg_s_axi_awready;

assign s_axi_wready  = reg_s_axi_wready;

assign s_axi_bresp   = reg_s_axi_bresp;
assign s_axi_bvalid  = reg_s_axi_bvalid;

assign s_axi_arready = reg_s_axi_arready;

assign s_axi_rdata   = reg_s_axi_rdata;
assign s_axi_rresp   = reg_s_axi_rresp;
assign s_axi_rvalid  = reg_s_axi_rvalid;

// Input registers for Write address

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_awaddr <= 8'b0;
    end else if (s_axi_awvalid == 1'b1) begin
        reg_s_axi_awaddr <= s_axi_awaddr;
    end
end

always @(*) begin
    if(reg_s_axi_awaddr[1:0] != 2'b00) begin
        reg_s_axi_awaddr_is_good <= 1'b0;
    end else begin
        case(reg_s_axi_awaddr[7:2])
            // Valid writing addresses
            6'b000000 : begin // Initialize
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b000100 : begin // Duplex Simple
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b001000 : begin // Duplex Simple Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b001001 : begin // Duplex Simple Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b001010 : begin // Duplex Simple Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b001011 : begin // Duplex Simple Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b001100 : begin // Duplex Encrypt
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b010000 : begin // Duplex Encrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b010001 : begin // Duplex Encrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b010010 : begin // Duplex Encrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b010011 : begin // Duplex Encrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b010100 : begin // Duplex Decrypt
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b011000 : begin // Duplex Decrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b011001 : begin // Duplex Decrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b011010 : begin // Duplex Decrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b011011 : begin // Duplex Decrypt Incomplete
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            6'b011100 : begin // Duplex Squeeze
                reg_s_axi_awaddr_is_good <= 1'b1;
            end
            default: begin // Not valid address
                reg_s_axi_awaddr_is_good <= 1'b0;
            end
        endcase
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_awprot <= 3'b0;
    end else if (s_axi_awvalid == 1'b1) begin
        reg_s_axi_awprot <= s_axi_awprot;
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_awvalid <= 1'b0;
    end else if ((reg_s_axi_awvalid == 1'b0) || (reg_s_axi_wvalid == 1'b1)) begin
        reg_s_axi_awvalid <= s_axi_awvalid;
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_awready <= 1'b0;
    end else begin
        reg_s_axi_awready <= 1'b1;
    end
end

// Input registers for Write data

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_wdata <= 32'b0;
    end else if (s_axi_wvalid == 1'b1) begin
        reg_s_axi_wdata <= s_axi_wdata;
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_wstrb <= 3'b0;
    end else if (s_axi_wvalid == 1'b1) begin
        reg_s_axi_wstrb <= s_axi_wstrb;
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_wvalid <= 1'b0;
    end else if ((reg_s_axi_wvalid == 1'b0) || (reg_s_axi_awvalid == 1'b1)) begin
        reg_s_axi_wvalid <= s_axi_wvalid;
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_wready <= 1'b0;
    end else begin
        reg_s_axi_wready <= 1'b1;
    end
end

assign reg_s_axi_wdata_number_bytes = reg_s_axi_awaddr[3:2];

// Response channel

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_bresp <= 2'b00;
    end else if ((reg_s_axi_awvalid == 1'b1) && (reg_s_axi_wvalid == 1'b1)) begin
        if (reg_s_axi_awaddr_is_good == 1'b0) begin
            reg_s_axi_bresp <= 2'b10;
        end else begin
            reg_s_axi_bresp <= 2'b00;
        end
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_bvalid <= 1'b0;
    end else begin 
        if ((reg_s_axi_awvalid == 1'b1) && (reg_s_axi_wvalid == 1'b1)) begin
            reg_s_axi_bvalid <= 1'b1;
        end else if ((reg_s_axi_bvalid == 1'b1) && (s_axi_bready == 1'b1)) begin
            reg_s_axi_bvalid <= 1'b0;
        end
    end
end

// Read Address related signals

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_araddr <= 8'b0;
    end else if (s_axi_arvalid == 1'b1) begin
        reg_s_axi_araddr <= s_axi_araddr;
    end
end

always @(*) begin
    if(reg_s_axi_araddr[1:0] != 2'b00) begin
        reg_s_axi_araddr_is_good <= 1'b0;
    end else begin
        case(reg_s_axi_araddr[7:2])
            // Valid writing addresses
            6'b000000 : begin // Initialize
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b000100 : begin // Duplex Simple
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b001000 : begin // Duplex Simple Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b001001 : begin // Duplex Simple Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b001010 : begin // Duplex Simple Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b001011 : begin // Duplex Simple Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b001100 : begin // Duplex Encrypt
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b010000 : begin // Duplex Encrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b010001 : begin // Duplex Encrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b010010 : begin // Duplex Encrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b010011 : begin // Duplex Encrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b010100 : begin // Duplex Decrypt
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b011000 : begin // Duplex Decrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b011001 : begin // Duplex Decrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b011010 : begin // Duplex Decrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b011011 : begin // Duplex Decrypt Incomplete
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b011100 : begin // Duplex Squeeze
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            6'b100000 : begin // Read buffer
                reg_s_axi_araddr_is_good <= 1'b1;
            end
            default: begin // Not valid address
                reg_s_axi_araddr_is_good <= 1'b0;
            end
        endcase
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_arprot <= 3'b0;
    end else if (s_axi_arvalid == 1'b1) begin
        reg_s_axi_arprot <= s_axi_arprot;
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_arvalid <= 1'b0;
    end else begin
        reg_s_axi_arvalid <= s_axi_arvalid;
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_arready <= 1'b0;
    end else begin
        reg_s_axi_arready <= 1'b1;
    end
end

// Read Data related signals

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_rdata <= 32'b0;
    end else begin
        if (reg_s_axi_arvalid == 1'b1) begin
            if (reg_s_axi_araddr[7:4] == 4'h8) begin
                reg_s_axi_rdata <= cipher_buffer_out;
            end else begin
                reg_s_axi_rdata <= 32'b0;
            end
        end
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_rresp <= 2'b0;
    end else if (reg_s_axi_arvalid == 1'b1) begin
        if(reg_s_axi_araddr_is_good == 1'b0) begin
            reg_s_axi_rresp <= 2'b10;
        end else begin
            reg_s_axi_rresp <= 2'b00;
        end
    end
end

always @(posedge aclk or negedge aresetn) begin
    if (aresetn == 1'b0) begin
        reg_s_axi_rvalid <= 1'b0;
    end else begin
        if (reg_s_axi_arvalid == 1'b1) begin
            reg_s_axi_rvalid <= 1'b1;
        end else if (reg_s_axi_rvalid == 1'b1 && s_axi_rready == 1'b1) begin
            reg_s_axi_rvalid <= 1'b0;
        end
    end
end

always @(*) begin
    if((reg_s_axi_awaddr_is_good == 1'b1) && (reg_s_axi_awvalid == 1'b1) && (reg_s_axi_wvalid == 1'b1)) begin
        cipher_start_operation <= 1'b1;
    end else begin
        cipher_start_operation <= 1'b0;
    end
end

assign cipher_operation_type = reg_s_axi_awaddr[7:4];
assign cipher_buffer_in = reg_s_axi_wdata;
assign cipher_buffer_in_size = reg_s_axi_wdata_number_bytes;

subterranean_simple_no_communication
cipher (
    .clk(aclk),
    .arstn(aresetn),
    .start_operation(cipher_start_operation),
    .operation_type(cipher_operation_type),
    .buffer_in(cipher_buffer_in),
    .buffer_in_size(cipher_buffer_in_size),
    .buffer_out(cipher_buffer_out),
    .core_free(cipher_core_free),
    .core_finish(cipher_core_finish)
);

endmodule