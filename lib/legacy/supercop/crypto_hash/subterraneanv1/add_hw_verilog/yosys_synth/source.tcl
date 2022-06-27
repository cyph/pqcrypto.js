set source_folder "../verilog_source/"
# List all files in the source folder
set source_file_names {"subterranean_round.v" "subterranean_round_with_communication.v" "subterranean_simple_no_communication.v" "subterranean_simple_axi4_lite.v"}
foreach source_file_name ${source_file_names} {yosys read_verilog [expr {"${source_folder}${source_file_name}"}]}
