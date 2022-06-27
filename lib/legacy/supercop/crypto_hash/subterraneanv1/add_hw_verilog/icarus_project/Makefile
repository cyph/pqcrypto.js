.PHONY: clean

ICARUS_VERILOG_SIMULATOR=iverilog
ICARUS_VERILOG_VVP=vvp

VERILOG_SOURCE_FOLDER=../verilog_source

SUBTERRANEAN_ROUND_SOURCE=\
	$(VERILOG_SOURCE_FOLDER)/subterranean_round.v

TB_SUBTERRANEAN_ROUND_SOURCE=\
	$(SUBTERRANEAN_ROUND_SOURCE)\
	$(VERILOG_SOURCE_FOLDER)/tb_subterranean_round.v

SUBTERRANEAN_ROUND_WITH_COMMUNICATION_SOURCE=\
	$(SUBTERRANEAN_ROUND_SOURCE)\
	$(VERILOG_SOURCE_FOLDER)/subterranean_round_with_communication.v

TB_SUBTERRANEAN_ROUND_WITH_COMMUNICATION_SOURCE=\
	$(SUBTERRANEAN_ROUND_WITH_COMMUNICATION_SOURCE)\
	$(VERILOG_SOURCE_FOLDER)/tb_subterranean_round_with_communication.v

SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_SOURCE=\
	$(SUBTERRANEAN_ROUND_SOURCE)\
	$(VERILOG_SOURCE_FOLDER)/subterranean_simple_no_communication.v

TB_SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_HASH=\
	$(SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_SOURCE)\
	$(VERILOG_SOURCE_FOLDER)/tb_subterranean_simple_no_communication_hash.v

TB_SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_AEAD=\
	$(SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_SOURCE)\
	$(VERILOG_SOURCE_FOLDER)/tb_subterranean_simple_no_communication_aead.v

SUBTERRANEAN_SIMPLE_AXI4_LITE=\
	$(SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_SOURCE)\
	$(VERILOG_SOURCE_FOLDER)/subterranean_simple_axi4_lite.v

TB_SUBTERRANEAN_SIMPLE_AXI4_LITE_HASH=\
	$(SUBTERRANEAN_SIMPLE_AXI4_LITE)\
	$(VERILOG_SOURCE_FOLDER)/tb_subterranean_simple_axi4_lite_hash.v

TB_SUBTERRANEAN_SIMPLE_AXI4_LITE_AEAD=\
	$(SUBTERRANEAN_SIMPLE_AXI4_LITE)\
	$(VERILOG_SOURCE_FOLDER)/tb_subterranean_simple_axi4_lite_aead.v

tb_subterranean_round: $(TB_SUBTERRANEAN_ROUND_SOURCE)
	$(ICARUS_VERILOG_SIMULATOR) -s tb_subterranean_round -o tb_subterranean_round_design $(TB_SUBTERRANEAN_ROUND_SOURCE)
	$(ICARUS_VERILOG_VVP) tb_subterranean_round_design

tb_subterranean_round_with_communication: $(TB_SUBTERRANEAN_ROUND_WITH_COMMUNICATION_SOURCE)
	$(ICARUS_VERILOG_SIMULATOR) -s tb_subterranean_round_with_communication -o tb_subterranean_round_with_communication_design $(TB_SUBTERRANEAN_ROUND_WITH_COMMUNICATION_SOURCE)
	$(ICARUS_VERILOG_VVP) tb_subterranean_round_with_communication_design

tb_subterranean_simple_no_communication_hash: $(TB_SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_HASH)
	$(ICARUS_VERILOG_SIMULATOR) -s tb_subterranean_simple_no_communication_hash -o tb_subterranean_simple_no_communication_hash_design $(TB_SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_HASH)
	$(ICARUS_VERILOG_VVP) tb_subterranean_simple_no_communication_hash_design

tb_subterranean_simple_no_communication_aead: $(TB_SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_AEAD)
	$(ICARUS_VERILOG_SIMULATOR) -s tb_subterranean_simple_no_communication_aead -o tb_subterranean_simple_no_communication_aead_design $(TB_SUBTERRANEAN_SIMPLE_NO_COMMUNICATION_AEAD)
	$(ICARUS_VERILOG_VVP) tb_subterranean_simple_no_communication_aead_design

tb_subterranean_simple_axi4_lite_hash: $(TB_SUBTERRANEAN_SIMPLE_AXI4_LITE_HASH)
	$(ICARUS_VERILOG_SIMULATOR) -s tb_subterranean_simple_axi4_lite_hash -o tb_subterranean_simple_axi4_lite_hash_design $(TB_SUBTERRANEAN_SIMPLE_AXI4_LITE_HASH)
	$(ICARUS_VERILOG_VVP) tb_subterranean_simple_axi4_lite_hash_design

tb_subterranean_simple_axi4_lite_aead: $(TB_SUBTERRANEAN_SIMPLE_AXI4_LITE_AEAD)
	$(ICARUS_VERILOG_SIMULATOR) -s tb_subterranean_simple_axi4_lite_aead -o tb_subterranean_simple_axi4_lite_aead_design $(TB_SUBTERRANEAN_SIMPLE_AXI4_LITE_AEAD)
	$(ICARUS_VERILOG_VVP) tb_subterranean_simple_axi4_lite_aead_design


clean:
	@echo "Cleaning up..."
	rm -f "dump.vcd"
	rm -f "tb_subterranean_round_design"
	rm -f "tb_subterranean_round_with_communication_design"
	rm -f "tb_subterranean_simple_no_communication_hash_design"
	rm -f "tb_subterranean_simple_no_communication_aead_design"
	rm -f "tb_subterranean_simple_axi4_lite_hash_design"
	rm -f "tb_subterranean_simple_axi4_lite_aead_design"
	@echo "Cleaning done."