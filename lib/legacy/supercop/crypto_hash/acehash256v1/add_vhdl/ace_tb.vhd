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

use std.textio.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.numeric_std.all;

use work.util_unsynth.all;
use work.ace_unsynth.all;
use work.ace_pkg.all;

--use work.util_estream.all;
--use work.util_unsynth.all;

--use work.wg_gf_pkg.all;
--use work.wg_pkg.all;
--use work.wg_unsynth_pkg.all;
--use work.wg_config_pkg.all;

entity ace_tb is
  generic (
        period            : real    --:= 10.0
  );
end entity;

architecture main of ace_tb is

  constant EDH          : std_logic_vector(0 to 2) := "111"; -- ENCRYPTION / DECRYPTION / HASH: '0' = don't do, '1' = do
  constant stim_file_path   : string := "dp_tv/nist_test.tv";
  constant output_file_path : string := "dp_tv/output.tv";





  constant clk_period   : time := period * 1 ns;
  constant hold         : time := clk_period / 2;
  constant after_reset  : time := 0 * clk_period;        -- delay between reset and loading
  constant load_delay   : time := 0 * clk_period;        -- delay between K, N data
  constant init_delay   : time := 0 * clk_period;        -- delay between load permutation and initialization
  constant procad_delay : time := 0 * clk_period;        -- AD data delay
  constant enc_delay    : time := 0 * clk_period;        -- encryption data delay
  constant dec_delay    : time := 0 * clk_period;        -- decryption data delay
  constant absorb_delay : time := 0 * clk_period;        -- delay between absorb data
  constant mode_delay   : time := 0 * clk_period;        -- delay between two modes

  signal clk            : std_logic := '0';
  signal reset          : std_logic := '0';

  signal i_mode         : mode_ty;
  signal ctl_control    : ace_ctl_ty;
  signal ctl_onehot     : onehot_ty;  

  signal i_const        : lfsr_c_output;
  signal i_dom_sep      : domsep_ty; 
  signal i_valid        : std_logic := '0';
  signal i_data         : word;
  signal ctl_count      : count_ty;
  signal o_valid,
         ctl_valid,
         o_ready,
         i_padding      : std_logic;
  signal o_data         : word;
        
  file output_file      : text open write_mode is output_file_path;

  type tb_state_ty is (tbEncrypt, tbDecrypt, tbTag, tbHash, tbNull);
  signal tb_state : tb_state_ty;

  signal bits_pad       : natural;

begin

  uut : entity work.ace port map
      ( clk       => clk
      , reset     => reset
      , i_mode    => i_mode
      , i_dom_sep => i_dom_sep
      , i_valid   => i_valid
      , i_data    => i_data
      , i_padding => i_padding
      , o_valid   => o_valid
      , o_ready   => o_ready
      , o_data    => o_data
      );

  clk <= not clk after clk_period / 2;

  reading_proc : process
    variable msg  : line;
    variable done : boolean;
  begin

    done := false;
    while not(done) loop
      wait until rising_edge(clk);
        done := (o_valid = '1');
      end loop;

      if tb_state = tbEncrypt then
        write(  msg, "Ciphertext: " & to_hex_string_normal( o_data( 0 to bits_pad-1 ) ) );
      elsif tb_state = tbDecrypt then
        write(  msg, "Plaintext:  " & to_hex_string_normal( o_data( 0 to bits_pad-1 ) ) );
      elsif tb_state = tbTag then
        write(  msg, "Tag:            " & to_hex_string_normal( o_data ) );
      elsif tb_state = tbHash then
        write(  msg, "Hash:       " & to_hex_string_normal( o_data ) );
      else
        write(  msg, "N/A:      " & to_hex_string_normal( o_data ) );
      end if;

    writeline( output_file, msg);

  end process;

  stimulus_proc : process
    variable num_bits   : natural;
    variable k0,k1,n0,
             n1,ad,m,
             m_next,iv  : std_logic_vector(0 to word_sz - 1);
    variable key_stim   : std_logic_vector(0 to key_sz - 1);
    variable nonce_stim : std_logic_vector(0 to nonce_sz  - 1);
    variable done       : boolean;
    variable i, j       : natural;
    variable data_buf   : word;
    variable data128    : std_logic_vector(0 to 127);
    variable tag        : tag_ty;
  begin


    ------------------------- SIGNAL DEFAULTS ----------------------------
    i_padding <= '0';    
    i_data    <= (others => 'X');  
    i_dom_sep <= (others => 'X');
    tb_state  <= tbNull;

    if EDH(0) = '1' then

      ------------------------------------------------------------------------------
      ---------------------------------- Encryption --------------------------------
      ------------------------------------------------------------------------------

      -------------------------------- Reset -------------------------------

      wait for clk_period;
      drive_reset( clk, reset, hold );
      wait until rising_edge(clk);

      -------------------------------- Load --------------------------------

      report( "LOADING PHASE" );
      read_key_nonce( key_stim, nonce_stim, stim_file_path );

      k0 := key_stim(0 to word_sz - 1);
      k1 := key_stim(word_sz to key_sz - 1);
      n0 := nonce_stim(0 to word_sz - 1);
      n1 := nonce_stim(word_sz to key_sz - 1);

      i_mode <= encrypt_mode;


      wait for hold;
      drive_data( clk, k0, hold, i_data, i_valid );        wait for load_delay;
      drive_data( clk, k1, hold, i_data, i_valid );        wait for load_delay;
      drive_data( clk, n0, hold, i_data, i_valid );        wait for load_delay;
      drive_data( clk, n1, hold, i_data, i_valid );

      -------------------------------- LoadPerm ------------------------------
      report( "PERMUTATION AFTER LOAD" );
      wait until o_ready = '1' and rising_edge(clk); 
      wait for init_delay;

      ---------------------------------- Init --------------------------------
      report( "INITIALIZATION PHASE" );

      i_dom_sep <= "00";

      drive_data( clk, k0, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);
      drive_data( clk, k1, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);

      ---------------------------------- ProcAD --------------------------------
      report( "AD PROCESSING PHASE" );

      i_dom_sep <= "01";    
      drive_all(AD_TAG, stim_file_path, hold, procad_delay, false, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);
      
      ---------------------------------- Encrypt ------------------------------
      report( "ENCRYPTION PHASE" );

      tb_state <= tbEncrypt;

      i_dom_sep <= "10";
      drive_all(PLAINTEXT_TAG, stim_file_path, hold, enc_delay, false, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);
  
      ---------------------------------- Final --------------------------------
      report( "FINALIZATION PHASE" );
      i_dom_sep <= "00";

      drive_data( clk, k0, hold, i_data, i_valid );
      wait until o_ready = '1' and rising_edge(clk);
      drive_data( clk, k1, hold, i_data, i_valid );

      ---------------------------------- Tag ----------------------------------
      report( "TAG PHASE" );
      tb_state <= tbTag;
      wait until o_ready = '1' and rising_edge(clk);

    end if;

      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------
      ---------------------------------- Decryption --------------------------------
      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------

    if EDH(1) = '1' then
      if EDH(0) = '1' then wait for mode_delay; end if;
      ------------------------- SIGNAL DEFAULTS ----------------------------
    
      i_data <= (others => 'X');  
      i_dom_sep   <= (others => 'X');

      ------------------------ READING STIM FILES  -------------------------

      -------------------------------- Reset -------------------------------

      wait for clk_period;
      drive_reset( clk, reset, hold );
      wait until rising_edge(clk);

      -------------------------------- Load --------------------------------

    report( "LOADING PHASE" );
    read_key_nonce( key_stim, nonce_stim, stim_file_path );

    k0 := key_stim(0 to word_sz - 1);
    k1 := key_stim(word_sz to key_sz - 1);
    n0 := nonce_stim(0 to word_sz - 1);
    n1 := nonce_stim(word_sz to key_sz - 1);

    i_mode <= decrypt_mode;

    wait for hold;
    drive_data( clk, k0, hold, i_data, i_valid );        wait for load_delay;
    drive_data( clk, k1, hold, i_data, i_valid );        wait for load_delay;
    drive_data( clk, n0, hold, i_data, i_valid );        wait for load_delay;
    drive_data( clk, n1, hold, i_data, i_valid );


   -------------------------------- LoadPerm ------------------------------
    report( "LOADING PERMUTATION" );    
    wait until o_ready = '1' and rising_edge(clk); 
    wait for init_delay;

    ---------------------------------- Init --------------------------------

    report( "INITIALIZATION PHASE" );

    i_dom_sep <= "00";

    drive_data( clk, k0, hold, i_data, i_valid );
    wait until o_ready = '1' and rising_edge(clk);
    drive_data( clk, k1, hold, i_data, i_valid );
    wait until o_ready = '1' and rising_edge(clk);

   ---------------------------------- ProcAD --------------------------------

    report( "AD PROCESSING PHASE" );

    i_dom_sep <= "01";    
    drive_all(AD_TAG, stim_file_path, hold, procad_delay, false, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);

   
    ---------------------------------- Decrypt ------------------------------

    report( "DECRYPTION PHASE" );

    tb_state <= tbDecrypt;

    i_dom_sep <= "10";
    drive_all(CIPHERTEXT_TAG, stim_file_path, hold, enc_delay, true, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);    

    ---------------------------------- Final --------------------------------

    report( "FINALIZATION PHASE" );
    i_dom_sep <= "00";

    drive_data( clk, k0, hold, i_data, i_valid );
    wait until o_ready = '1' and rising_edge(clk);
    drive_data( clk, k1, hold, i_data, i_valid );

    ---------------------------------- Tag ----------------------------------

    report( "TAG PHASE" );
    tb_state <= tbTag;
    wait until o_ready = '1' and rising_edge(clk);



  end if;
      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------
      ------------------------------------ HASH ------------------------------------
      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------
      ------------------------------------------------------------------------------

  if EDH(2) = '1' then
    if EDH(1) = '1' or EDH(0) = '1' then wait for mode_delay; end if;

    ------------------------- SIGNAL DEFAULTS ----------------------------
    i_data    <= (others => 'X');  
    i_dom_sep <= (others => 'X');

    ------------------------ READING STIM FILES  -------------------------
    iv := x"8040400000000000";

    -------------------------------- Reset -------------------------------
    wait for clk_period;
    drive_reset( clk, reset, hold );
    wait until rising_edge(clk);

    -------------------------------- Load --------------------------------
    report( "LOADING PHASE" );
    i_mode <= absorb_mode;

    wait for hold;
    drive_data( clk, iv, hold, i_data, i_valid );        wait for load_delay;

   -------------------------------- LoadPerm ------------------------------
    report( "LOADING PERMUTATION" );    
    wait until o_ready = '1' and rising_edge(clk); 
    wait for absorb_delay;

    --------------------------------- Absorb -------------------------------

    i_dom_sep <= "00";
    drive_all(PLAINTEXT_TAG, stim_file_path, hold, enc_delay, false, clk, o_ready, o_data, i_data, i_valid, i_padding, bits_pad);

    --------------------------------- Squeeze -------------------------------

    wait for hold;

    tb_state <= tbHash;
    i_mode   <= squeeze_mode;

  end if;

    wait until o_ready = '1' and rising_edge(clk);

    assert false
    report ("SIMULATION IS FINISHED")
    severity failure;
        

  end process;

end architecture main;






