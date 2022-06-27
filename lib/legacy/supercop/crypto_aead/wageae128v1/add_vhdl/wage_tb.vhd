-- This work is licensed under a Creative Commons
-- Attribution-NonCommercial-ShareAlike 4.0 International License.
-- http://creativecommons.org/licenses/by-nc-sa/4.0

-- Mark D. Aagaard
-- Riham AlTawy
-- Guang Gong
-- Kalikinkar Mandal
-- Raghvendra Rohit
-- Marat Sattarov
-- http://comsec.uwaterloo.ca


-- This is a human-readable summary of (and not a substitute for) the license. 
-- You are free to:

--     Share — copy and redistribute the material in any medium or format
--     Adapt — remix, transform, and build upon the material

--     The licensor cannot revoke these freedoms as long as you follow
--     the license terms.

-- Under the following terms:

--     Attribution — You must give appropriate credit, provide a link to
--     the license, and indicate if changes were made. You may do so in
--     any reasonable manner, but not in any way that suggests the
--     licensor endorses you or your use.

--     NonCommercial — You may not use the material for commercial
--     purposes.

--     ShareAlike — If you remix, transform, or build upon the material,
--     you must distribute your contributions under the same license as
--     the original.

--     No additional restrictions — You may not apply legal terms or
--     technological measures that legally restrict others from doing
--     anything the license permits.

---------------------------------
--- TESTBENCH  for module WAGElfsr
--  test load correctness 

--  *passed*

---------------------------


LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.all;
USE STD.textio.all;
USE ieee.std_logic_textio.all;

use work.util_unsynth.all;
use work.wage_unsynth.all;
use work.wage_pkg.all;    


ENTITY wage_tb IS
  generic (
      period            : real    := 10.0
  );
END wage_tb; 

ARCHITECTURE main OF wage_tb IS

  constant ED           : std_logic_vector(0 to 1) := "11";  -- Encryption | Decryption

  constant stim_file_path   : string := "dp_tv/nist_test.tv";
  constant output_file_path : string := "dp_tv/output.tv";

  file output_file      : text open write_mode is output_file_path;
  file stim_file        : text;


  constant clk_period   : time  := period * 1 ns;
  constant hold         : time  := clk_period / 2;
  constant after_reset  : time  := 0 * clk_period;  -- delay between reset and loading
  constant load_delay   : time  := 0 * clk_period;  -- delay between K, N data
  constant init_delay   : time  := 0 * clk_period;  -- delay between load permutation and initialization
  constant procad_delay : time  := 0 * clk_period;  -- AD data delay
  constant enc_delay    : time  := 0 * clk_period;  -- encryption data delay
  constant dec_delay    : time  := 0 * clk_period;  -- decryption data delay
  constant mode_delay   : time  := 0 * clk_period;  -- delay between two modes
  

   --Inputs
  signal clk, reset : std_logic := '0'; 

  
  signal tag_data_2 : std_logic_vector(0 to 127);

  type tb_state_ty is (tbLoad, tbInit, tbProcAD, tbEncrypt, tbDecrypt, tbFinal, tbTag, tbNull);
  signal tb_state_2 : tb_state_ty;   
  signal wage_state_f_2 : state_array;  
  signal wage_state_v_2 : std_logic_vector(N*gf_dim-1 downto 0);
  signal wage_state_a_2 : state_array_temp;
  signal tag_valid  : std_logic := '0';



  signal i_data, o_data : word;
  signal o_valid, i_valid, i_padding, o_ready : std_logic;
  signal i_dom_sep : domsep_ty;
  signal i_mode    : mode_ty;

  signal bits_pad  : natural;

BEGIN  



  reading_proc_2 : process
    variable msg    : line;
    variable done, skip_write  : boolean;
    --alias o_data_alias is << signal .wage_dp_tb.uut.o_data : data_io >>;
    variable o_data_var : std_logic_vector(0 to 63);
  begin

    skip_write := false;
    done := false;
    while not(done) loop
      wait until rising_edge(clk);
      done := (o_valid = '1' or tag_valid = '1');
    end loop;

    o_data_var := o_data;

    if tb_state_2 = tbEncrypt then
      write(  msg, "Ciphertext:      " & to_hex_string_normal( o_data( 0 to bits_pad-1 ) ));
    elsif tb_state_2 = tbDecrypt then
      write(  msg, "Plaintext:       " & to_hex_string_normal( o_data( 0 to bits_pad-1 ) ));
    elsif tb_state_2 = tbTag then
      if tag_valid = '1' then
        write(  msg, "Tag:           " & to_hex_string_normal( tag_data_2 ));
      else
        skip_write := true;
      end if;
    else
      skip_write := true;
    end if;

    if not(skip_write) then writeline( output_file, msg); end if;

  end process;


 
  clk <= not(clk) after clk_period / 2;


   -- Instantiate the Unit Under Test (UUT)
  uut: entity work.wage port map 
    (  clk       => clk
    ,  reset     => reset
    ,  i_mode    => i_mode
    ,  i_dom_sep => i_dom_sep
    ,  i_valid   => i_valid
    ,  i_data    => i_data
    ,  i_padding => i_padding
    ,  o_valid   => o_valid
    ,  o_ready   => o_ready
    ,  o_data    => o_data
    );

  stim_proc_2: process
    variable num_bits         : natural;
    variable file_tag         : tag_ty;
    variable key_stim,
             nonce_stim,
             data128          : std_logic_vector(0 to 127);
    variable ad, m, k0,
             k1, data         : std_logic_vector(0 to 63);
    variable tag_vector       : data_io_vector(0 to 8);
    variable kn_load_vector_2 : data_io_vector(0 to 8);

  begin  

    if ED(0) = '1' then
      ------------------------------------------------------------------------------
      ---------------------------------- Encryption --------------------------------
      ------------------------------------------------------------------------------

      tag_data_2 <= (others => 'X');
      i_valid  <= '0';
      i_padding  <= '0';

      tb_state_2 <= tbNull;

      -------------------------------- Reset -------------------------------

      wait for clk_period;
      drive_reset( clk, reset, hold );
      wait until rising_edge(clk);
  
      -------------------------------- Load --------------------------------
      report( "LOADING PHASE" );
      tb_state_2 <= tbLoad;

      i_dom_sep <= "00";

      read_key_nonce( key_stim, nonce_stim, stim_file_path );
      kn_load_vector_2 := kn_to_data_io_vector (key_stim, nonce_stim);
      k0 := key_stim(0 to 63);
      k1 := key_stim(64 to 127);

      for i in 0 to 8 loop
        data := data_io_to_m(kn_load_vector_2(i));
        drive_data( clk, data, hold, i_data, i_valid );
        wait for load_delay;
      end loop;
     
      -------------------------------- LoadPerm ------------------------------
      report( "PERMUTATION AFTER LOAD" );   
      wait until o_ready = '1' and rising_edge(clk); 
      wait for init_delay;

      ---------------------------------- Init --------------------------------
      report( "INITIALIZATION PHASE" );
      tb_state_2 <= tbInit;

      drive_data( clk, k0, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);
      drive_data( clk, k1, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);

      ---------------------------------- ProcAD --------------------------------
      report( "AD PROCESSING PHASE" );
      tb_state_2 <= tbProcAD;

      i_dom_sep <= "10";

      drive_all(AD_TAG, stim_file_path, hold, procad_delay, false, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);

      ---------------------------------- Encrypt ------------------------------
      report( "ENCRYPTION PHASE" );
      tb_state_2 <= tbEncrypt;

      i_dom_sep <= "01";
      i_mode <= encrypt_mode;

      drive_all(PLAINTEXT_TAG, stim_file_path, hold, enc_delay, false, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);

      ---------------------------------- Final --------------------------------
      report( "FINALIZATION PHASE" );
      tb_state_2 <= tbFinal;
  
      i_dom_sep <= "00";

      drive_data( clk, k0, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);

      drive_data( clk, k1, hold, i_data, i_valid );
      wait until o_valid = '1' and rising_edge(clk); wait for hold;

      ---------------------------------- Tag ----------------------------------
      report( "TAG PHASE" );
      tb_state_2 <= tbTag;

      for i in 0 to 8 loop
        tag_vector(i) := m_to_data_io(o_data);
        report("tag vector was read: " & to_hex_string_normal( o_data ) );
        wait until rising_edge(clk); wait for hold;
      end loop;

      tag_data_2 <= data_io_vector_to_tag(tag_vector);
      tag_valid <= '1';

      wait until rising_edge(clk); wait for hold;
      report("tag is: " & to_hex_string_normal( tag_data_2 ) );
      tag_valid <= '0';

      wait for mode_delay;

    end if;

    if ED(1) = '1' then
      ------------------------------------------------------------------------------
      ---------------------------------- Decryption --------------------------------
      ------------------------------------------------------------------------------
 
      tag_data_2 <= (others => 'X');
      i_valid    <= '0'; 
      i_padding  <= '0';
      tb_state_2 <= tbNull;

      -------------------------------- Reset -------------------------------

      wait for clk_period;
      drive_reset( clk, reset, hold );
      wait until rising_edge(clk);
  
      -------------------------------- Load --------------------------------
      report( "LOADING PHASE" );
      tb_state_2 <= tbLoad;

      i_dom_sep <= "00";

      read_key_nonce( key_stim, nonce_stim, stim_file_path );
      kn_load_vector_2 := kn_to_data_io_vector (key_stim, nonce_stim);
      k0 := key_stim(0 to 63);
      k1 := key_stim(64 to 127);

      for i in 0 to 8 loop
        data := data_io_to_m(kn_load_vector_2(i));
        drive_data( clk, data, hold, i_data, i_valid );
        wait for load_delay;
      end loop;
     
      -------------------------------- LoadPerm ------------------------------
      report( "PERMUTATION AFTER LOAD" );   
      wait until o_ready = '1' and rising_edge(clk); 
      wait for init_delay;

      ---------------------------------- Init --------------------------------
      report( "INITIALIZATION PHASE" );
      tb_state_2 <= tbInit;

      drive_data( clk, k0, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);
      drive_data( clk, k1, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);

      ---------------------------------- ProcAD --------------------------------
      report( "AD PROCESSING PHASE" );
      tb_state_2 <= tbProcAD;

      i_dom_sep <= "10";

      drive_all(AD_TAG, stim_file_path, hold, procad_delay, false, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);

      ---------------------------------- Decrypt ------------------------------
      report( "ENCRYPTION PHASE" );
      tb_state_2 <= tbEncrypt;

      i_dom_sep <= "01";
      i_mode <= decrypt_mode;

      drive_all(CIPHERTEXT_TAG, stim_file_path, hold, enc_delay, true, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);

      ---------------------------------- Final --------------------------------
      report( "FINALIZATION PHASE" );
      tb_state_2 <= tbFinal;
  
      i_dom_sep <= "00";

      drive_data( clk, k0, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);

      drive_data( clk, k1, hold, i_data, i_valid );
      wait until o_valid = '1' and rising_edge(clk); wait for hold;

      ---------------------------------- Tag ----------------------------------
      report( "TAG PHASE" );
      tb_state_2 <= tbTag;

      for i in 0 to 8 loop
        tag_vector(i) := m_to_data_io(o_data);
        report("tag vector was read: " & to_hex_string_normal( o_data ) );
        wait until rising_edge(clk); wait for hold;
      end loop;

      tag_data_2 <= data_io_vector_to_tag(tag_vector);
      tag_valid <= '1';

      wait until rising_edge(clk); wait for hold;
      report("tag is: " & to_hex_string_normal( tag_data_2 ) );
      tag_valid <= '0';

      wait for mode_delay;

    end if;

      assert false
      report ("SIMULATION IS FINISHED")
      severity failure;

  end process;

end;
