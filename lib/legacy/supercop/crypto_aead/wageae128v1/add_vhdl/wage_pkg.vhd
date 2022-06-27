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

package wage_pkg is

  -- for the wage_lfsr 
  constant gf_dim : integer := 7;
  constant N : integer := 37; -- number of stages
  constant D : integer := 10; -- number of data inputs/outputs
  subtype gf_elem is std_logic_vector( 0 to gf_dim-1 );
  type state_array is array ( N-1 downto 0 ) of gf_elem;
  type data_io is array ( D-1 downto 0 ) of gf_elem;
  type data_io_vector is array ( natural range <> ) of data_io;
  type gf_vector is array ( natural range <> ) of gf_elem;
  subtype word is std_logic_vector( 0 to 63 );

  -- for the lfsr to produce the constants
  constant lfsr_c_sz:  integer := 7;
  type state_array_c is array ( lfsr_c_sz - 1 downto 0 ) of std_logic;

  subtype mode_ty is std_logic;     -- top lvl input
  constant encrypt_mode : mode_ty := '0';
  constant decrypt_mode : mode_ty := '1';
  
  subtype domsep_ty is std_logic_vector( 0 to 1 );    -- top lvl input

  constant bits_counter : natural := 8;
  subtype count_ty is unsigned( bits_counter - 1 downto 0 );

  subtype  wage_ctl_ty is std_logic_vector( 7 downto 0 );  
  constant is_tag_idx        : natural := 0;
  constant sb_off_idx        : natural := 1;
  constant i_padding_reg_idx : natural := 2;
  constant lfsr_en_idx       : natural := 3;
  constant lfsr_c_reset_idx  : natural := 4;
  constant absorb_idx        : natural := 5;
  constant replace_idx       : natural := 6;
  constant load_idx          : natural := 7;

end wage_pkg;




 



