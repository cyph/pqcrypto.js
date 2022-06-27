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
use work.wage_pkg.all;

entity dp is
  port
    ( clk       : in  std_logic
    ; i_control : in  wage_ctl_ty
    ; i_dom_sep : in  domsep_ty
    ; i_data    : in  data_io
    ; o_data    : out data_io
   );
end dp;

architecture rtl of dp is
  signal i_s36, i_s29, i_s23, i_s18, i_s10, i_s4, o_s36, o_s34, o_s27, o_s18, o_s15, o_s8, wgp_left, wgp_right, rc1, rc0: gf_elem;  
  signal const: std_logic_vector( lfsr_c_sz downto 0 );
  signal lfsr_c_reset, lfsr_en,
         absorb, replace,
         load, sb_off,
         is_tag, i_padding_reg : std_logic;

begin

  is_tag        <= i_control( is_tag_idx );
  sb_off        <= i_control( sb_off_idx );
  i_padding_reg <= i_control( i_padding_reg_idx );
  lfsr_en       <= i_control( lfsr_en_idx );
  lfsr_c_reset  <= i_control( lfsr_c_reset_idx );
  absorb        <= i_control( absorb_idx );
  replace       <= i_control( replace_idx );
  load          <= i_control( load_idx );

  -- wage lfsr
  wage_state: entity work.wagelfsr port map ( clk, lfsr_en, absorb, replace, load, sb_off, is_tag, i_padding_reg, i_dom_sep, i_data, i_s36, i_s29, i_s23, i_s18, i_s10, i_s4, o_s36, o_s34, o_s27, o_s18, o_s15, o_s8, o_data );       

  -- 4x sboxes
  SB_left_1:  entity work.sb port map ( o_s34, i_s29 );
  SB_left_2:  entity work.sb port map ( o_s27, i_s23 );
  SB_right_1: entity work.sb port map ( o_s15, i_s10 );
  SB_right_2: entity work.sb port map ( o_s8, i_s4 );
  
  -- 2x WGP
  wgpleft:    entity work.dwgp(const_array) port map ( o_s36, wgp_left );
  wgpright:   entity work.dwgp(const_array) port map ( o_s18, wgp_right );
 
  -- lfsr_c for constants
  lfsrconst:  entity work.lfsr_c port map ( clk, lfsr_en, lfsr_c_reset, const );

  rc1 <= const( lfsr_c_sz downto 1 ); 
  rc0 <= const( lfsr_c_sz - 1 downto 0 );

  i_s36 <= wgp_left  xor rc1;
  i_s18 <= wgp_right xor rc0;

end;
