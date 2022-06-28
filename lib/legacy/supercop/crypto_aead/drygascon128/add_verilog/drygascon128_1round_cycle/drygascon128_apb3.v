//module generated using Spinal HDL, verified using VexRiscv - Murax SOC
module drygascon128_apb3 (
      input  [7:0] io_apb_PADDR,
      input  [0:0] io_apb_PSEL,
      input   io_apb_PENABLE,
      output  io_apb_PREADY,
      input   io_apb_PWRITE,
      input  [31:0] io_apb_PWDATA,
      output reg [31:0] io_apb_PRDATA,
      output  io_apb_PSLVERROR,
      input   toplevel_resetCtrl_systemReset,
      input   toplevel_io_mainClk);
  wire  _zz_1_;
  wire [31:0] core_dout;
  wire  core_idle;
  wire  selected;
  wire  askWrite;
  wire  askRead;
  wire  doWrite;
  wire  doRead;
  reg  start;
  reg [3:0] ds;
  reg [3:0] rounds;
  reg  rd_c;
  reg  wr_c;
  reg  rd_r;
  reg  wr_x;
  reg  wr_i;
  wire [31:0] ctrl;
  wire [31:0] zeroes;
  wire  ctrl_hit;
  drygascon128 core (
    .clk(toplevel_io_mainClk),
    .clk_en(_zz_1_),
    .rst(toplevel_resetCtrl_systemReset),
    .din(io_apb_PWDATA),
    .ds(ds),
    .wr_i(wr_i),
    .wr_c(wr_c),
    .wr_x(wr_x),
    .rounds(rounds),
    .start(start),
    .rd_r(rd_r),
    .rd_c(rd_c),
    .dout(core_dout),
    .idle(core_idle)
  );
  assign _zz_1_ = 1'b1;
  assign selected = (io_apb_PSEL[0] && io_apb_PENABLE);
  assign askWrite = (selected && io_apb_PWRITE);
  assign askRead = (selected && (! io_apb_PWRITE));
  assign doWrite = ((selected && io_apb_PREADY) && io_apb_PWRITE);
  assign doRead = ((selected && io_apb_PREADY) && (! io_apb_PWRITE));
  assign zeroes = (1'b0 ? (32'b11111111111111111111111111111111) : (32'b00000000000000000000000000000000));
  assign ctrl = {{{{core_idle,zeroes[30 : 9]},start},ds},rounds};
  assign io_apb_PSLVERROR = 1'b0;
  assign io_apb_PREADY = 1'b1;
  always @ (*) begin
    rd_c = 1'b0;
    wr_c = 1'b0;
    rd_r = 1'b0;
    wr_x = 1'b0;
    wr_i = 1'b0;
    io_apb_PRDATA = (32'b00001110101011011011111011101111);
    if(selected)begin
      case(io_apb_PADDR)
        8'b00000000 : begin
          io_apb_PRDATA = ctrl;
        end
        8'b00000100 : begin
          io_apb_PRDATA = core_dout;
          rd_c = (! doWrite);
          wr_c = doWrite;
        end
        8'b00001000 : begin
          io_apb_PRDATA = core_dout;
          rd_r = (! doWrite);
          wr_i = doWrite;
        end
        8'b00001100 : begin
          wr_x = doWrite;
        end
        default : begin
        end
      endcase
    end
  end

  assign ctrl_hit = (selected && (io_apb_PADDR == (8'b00000000)));
  always @ (posedge toplevel_io_mainClk or posedge toplevel_resetCtrl_systemReset) begin
    if (toplevel_resetCtrl_systemReset) begin
      start <= 1'b0;
      ds <= (4'b0000);
      rounds <= (4'b1011);
    end else begin
      start <= 1'b0;
      if((doWrite && ctrl_hit))begin
        rounds <= io_apb_PWDATA[3 : 0];
        ds <= io_apb_PWDATA[7 : 4];
        start <= io_apb_PWDATA[8];
      end
    end
  end

endmodule
