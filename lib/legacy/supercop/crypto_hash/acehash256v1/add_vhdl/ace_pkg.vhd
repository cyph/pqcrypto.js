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

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package ace_pkg is

  --for constants
  constant lfsr_c_sz    :  integer := 7;
  subtype lfsr_c_output is std_logic_vector(0 to lfsr_c_sz+2);

  ------------------------------------------------------------
  
  constant half_word_sz : natural := 32;
  constant word_sz      : natural := 2*half_word_sz;  

  subtype half_word     is std_logic_vector( 0 to half_word_sz - 1 );
  subtype word          is std_logic_vector( 0 to word_sz - 1 );
  type word_vector  	is array( natural range <> ) of word; 
  type half_word_vector is array( natural range <> ) of half_word;

  ------------------------------------------------------------
  -- A, B, C, D, E

  constant state_sz          : natural := 320;
  constant word_max_idx      : natural := state_sz / word_sz - 1;
  constant half_word_max_idx : natural := state_sz / half_word_sz - 1;

  constant key_sz            : natural := 128;
  constant nonce_sz          : natural := 128;

  subtype word_state_ty is word_vector ( 0 to word_max_idx );
  constant a_idx : natural := 0;
  constant b_idx : natural := 1;
  constant c_idx : natural := 2;
  constant d_idx : natural := 3;
  constant e_idx : natural := 4;

  subtype half_word_data is  half_word_vector ( 0 to 1 );

  subtype half_word_state_ty is half_word_vector ( 0 to half_word_max_idx );
  constant a0_idx : natural := 1;
  constant a1_idx : natural := 0;
  constant b0_idx : natural := 3;
  constant b1_idx : natural := 2;
  constant c0_idx : natural := 5;
  constant c1_idx : natural := 4;
  constant d0_idx : natural := 7;
  constant d1_idx : natural := 6;
  constant e0_idx : natural := 9;
  constant e1_idx : natural := 8;
  
  function b2x( b : boolean ) return std_logic;
  function half_words_to_words( st : half_word_state_ty ) return word_state_ty;
  function words_to_half_words( st : word_state_ty ) return half_word_state_ty;

  ------------------------------------------------------------
  -- mode

  subtype mode_ty is std_logic_vector( 1 downto 0 );     -- top lvl input
  constant encrypt_mode : mode_ty := ( 1 => '0', 0 => '0' );
  constant decrypt_mode : mode_ty := ( 1 => '0', 0 => '1' );
  constant absorb_mode  : mode_ty := ( 1 => '1', 0 => '0' );
  constant squeeze_mode : mode_ty := ( 1 => '1', 0 => '1' );

  subtype domsep_ty is std_logic_vector( 1 downto 0 );    -- top lvl input

  -- derived control (from counter and more)
  subtype ace_ctl_ty is std_logic_vector( 7 downto 0 );  
  constant absorb_idx       : natural := 0;
  constant replace_idx      : natural := 1;
  constant output_idx       : natural := 2;
  constant endstep_idx      : natural := 3;
  constant permoff_idx      : natural := 4;
  constant squeeze_idx      : natural := 5;
  constant lfsr_c_reset_idx : natural := 6;
  constant lfsr_c_en_idx    : natural := 7;

  -- extras cntl for load, init, fin, tag, sqeeze
  subtype onehot_ty is std_logic_vector( 3 downto 0);		-- extrs cntl for load, init, fin, tag, sqeeze

  ------------------------------------------------------------
  -- round and step counters

  -- use last bit for end ACE perm - for o_ready
  -- -> i_valid will reset the counter!
  -- counter only runs if msb = 0
  constant bits_counter : natural := 8; 
  subtype count_ty is unsigned( bits_counter - 1 downto 0 );
  
  ----------------------------------------------------------------------
  -- standard vhdl operators

  -- function "sll"( a : half_word; n : natural ) return half_word;
  
  function onehot_rotate (a : onehot_ty) return onehot_ty;
  function vector_to_data ( st : half_word_data ) return word;
  function data_to_vector ( st : word ) return half_word_data;
  ----------------------------------------------------------------------
    
end package;

----------------------------------------------------------------------
--
----------------------------------------------------------------------

package body ace_pkg is

  function onehot_rotate (a : onehot_ty)
    return onehot_ty
  is
    variable z : onehot_ty;
  begin
      z(onehot_ty'high downto 1) := a(onehot_ty'high - 1 downto 0);
      z(0) := a(onehot_ty'high);
    return z;
  end function;

  function b2x( b : boolean ) return std_logic is
  begin
    if b then
      return '1';
    else
      return '0';
    end if;
  end function;  

  ------------------------------------------------------------
  -- standard vhdl operators cast to state
  ------------------------------------------------------------

--  function "sll"( a : half_word; n : natural ) return half_word is
--  begin
--    return half_word( std_logic_vector( a ) sll n );
--  end function;
  
  ------------------------------------------------------------
  -- state functions
  ------------------------------------------------------------

  function half_words_to_words( st : half_word_state_ty )	---- CHECK THIS!!!!!! PLEASE
    return word_state_ty
  is
    variable i : natural;
    variable z : word_state_ty;
  begin
    main_loop : for i in 0 to word_max_idx loop
     z(i)(0 to half_word_sz - 1)  := st(2*i);
     z(i)(half_word_sz to word_sz - 1)  := st(2*i+1);
    end loop;
  return z;
  end function;

  function words_to_half_words( st : word_state_ty )
    return half_word_state_ty
  is
    variable i : natural;
    variable z : half_word_state_ty;
  begin
    main_loop : for i in 0 to word_max_idx loop
      z(2*i)     := st(i)(0 to half_word_sz - 1);
      z(2*i+1) := st(i)(half_word_sz to word_sz - 1);
    end loop;
  return z;
  end function;

  function data_to_vector( st : word )
    return half_word_data
  is
    variable z : half_word_data;
  begin
    z(0) := st(0 to half_word_sz - 1);
    z(1) := st(half_word_sz to word_sz - 1);
    return z;
  end function;


  function vector_to_data( st : half_word_data )
    return word
  is
    variable z : word;
  begin
    z(0 to half_word_sz - 1) := st(0);
    z(half_word_sz to word_sz - 1) := st(1);
    return z;
  end function;


end package body;


