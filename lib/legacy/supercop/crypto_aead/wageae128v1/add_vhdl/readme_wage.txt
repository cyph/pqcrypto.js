------------ WAGE readme file ---------------
----- list of files for WAGE synthesis: -----

wage_pkg.vhd         -- main package
omega.vhd            -- multiplication by omega
wagelfsr.vhd         -- main WAGE lfsr
r_sb.vhd             -- s-box: one round R
q_sb.vhd             -- s-box: one transformation Q
not_sb.vhd           -- s-box: last two NOT gates
sb.vhd               -- s-box
lfsrc.vhd            -- lfsr for round constant generation
dwgp.vhd             -- dwgp entity
dwgp-const_array.vhd -- constant array dwgp architecture
dp.vhd               -- datapath
ctl.vhd              -- control (FSM)
wage.vhd             -- top level entity declaration
wage-rtl.vhd         -- top level architecture

-------------- pure datapath ---------------
wagelfsr_pure.vhd    -- main WAGE lfsr with most input/output multiplexers removed

----- additional attributes for Xilinx ISE: -----

dwgp.vhd
  -- prevent memory inference for Xilinx
  attribute ram_extract : string;
  attribute ram_extract of dwgp : entity is "yes";  --"no"; 
  
  attribute rom_extract : string;
  attribute rom_extract of dwgp : entity is "yes";  --"no"; 


wagelfsr.vhd
  -- prevent SRL LUT configuration for Xilinx
  attribute shreg_extract : string;
  attribute shreg_extract of sa : signal is "yes";  --"no";




----- additional files for simulation: -----

util_unsynth.vhd -- functions used in TB (general purpose)
wage_unsynth.vhd -- specific WAGE functions and procedures
wage_tb.vhd      -- WAGE testbench

----------- TB info (wage_tb.vhd): ---------

********
ED is a 2-bit constant used to select which modes to test

"10" - encryption only
"01" - decyption only
"11" - encryption and decryption

********
stim_file_path   -- stimulus file
output_file_path -- output file
********


------------ stimulus file format --------------
1 file = 1 set of Key, Nonce, AD, Plaintext and Ciphertext

K 00111122335588DD00111122335588DD   <--- 128 bits of Key (all 128 bits in a single line)
N 111122335588DD00111122335588DD00   <--- 128 bits of Nonce (all 128 bits in a single line)

A 1122335588DD00111122335588DD00     <--- from 4 to 128 bits of AD
P 335588DD00111122335588DD001111     <--- from 4 to 128 bits of Plaintext
C F9362385DC213A07CEFEF38C34CEFF     <--- from 4 to 128 bits of Ciphertext

--- padding is done by testbench
--- multiple lines for AD, Plaintext and Ciphertext are supported
