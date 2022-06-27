------------ ACE readme file ---------------
----- list of files for ACE synthesis: -----

ace_pkg.vhd     -- main package
sb_64.vhd       -- s-box with simeck
lfsr.vhd        -- lfsr for step / round constant generation
ctl.vhd         -- control (FSM)
dp.vhd          -- datapath
ace.vhd         -- top level entity declaration
ace-rtl.vhd     -- top level architecture

----- additional files for simulation: -----

util_unsynth.vhd -- functions used in TB (general purpose)
ace_unsynth.vhd  -- specific ACE functions and procedures used in TB
ace_tb.vhd       -- ACE testbench

-------------- pure datapath ---------------
dp_pure.vhd      -- datapath with most input/output multiplexers removed

----------- TB info (ace_tb.vhd): ----------

********
EDH is a 3-bit constant used to select which modes to test

"100" - encryption only
"010" - decyption only
"001" - hash only
"110" - encryption and decryption
etc.

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
