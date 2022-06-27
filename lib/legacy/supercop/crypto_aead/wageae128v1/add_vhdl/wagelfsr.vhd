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

-- 4 types of MUX and AND for tag 
entity wagelfsr is
  port
    ( clk, 
   -- ce,
      lfsr_en, absorb,
      replace, load,
      sb_off, is_tag        : in  std_logic
    ; i_padding_reg         : in  std_logic
    ; i_dom_sep             : in  domsep_ty
    ; i_data                : in  data_io
    ; i_s36, i_s29, i_s23   : in  gf_elem --  left half nonlinears (inputs)
    ; i_s18, i_s10, i_s4    : in  gf_elem -- right half nonlinears (inputs)  
    ; o_s36, o_s34, o_s27   : out gf_elem --  left half nonlinears (outputs)
    ; o_s18, o_s15, o_s8    : out gf_elem -- right half nonlinears (outputs)
    ; o_data                : out data_io
   );
	

	
end wagelfsr;

architecture rtl of wagelfsr is
  signal f, omega : gf_elem;  -- feedback
  signal i_data_9 : gf_elem;  -- for loading
  signal sa : state_array;    -- wage state 
  signal x,                   -- result of XORing with nonlinears
         ab,                  -- result of absorbing  
         y,                   -- choose between ab and sa/x 
         l  : state_array;    -- choose betweeb y and i_data
  
  -- for plain shift updates
  constant shiftplain : std_logic_vector(N-1 downto 0) := 
    (33 => '1', 32 => '1', 31 => '1', 30 => '1', 26 => '1', 25 => '1', 24 => '1', 22 => '1', 21 => '1', 20 => '1', 19 => '1', 
    17 => '1', 14 => '1', 13 => '1', 12 => '1', 11 => '1', 7 => '1', 6 => '1', 5 => '1', 3 => '1', 2 => '1', 1 => '1', others => '0'  );      
 
  signal replace_or_load : std_logic;

  -- prevent SRL LUT configuration  for Xilinx
  attribute shreg_extract : string;
  attribute shreg_extract of sa : signal is "yes"; --"no";

begin

  replace_or_load <= replace OR load;    -- stages with inputs D_9 , ... D_0

  -- outputs - nonlinears 2x(WGP, SB, SB)
  o_s36 <= sa(36);  o_s34 <= sa(34);   o_s27 <= sa(27); -- to left nonlinears
  o_s18 <= sa(18);  o_s15 <= sa(15);   o_s8  <= sa(8);  -- to right nonlinears

  -- output data
  o_data(9) <= ab(36);
  o_data(8) <= ab(35);
  o_data(7) <= ab(34);
  o_data(6) <= ab(28);
  o_data(5) <= ab(27);  -- also TAG --
  o_data(4) <= ab(18);
  o_data(3) <= ab(16);  -- also TAG --
  o_data(2) <= ab(15);  
  o_data(1) <= ab(9);  
  o_data(0) <= ab(8);   -- also TAG --

  -- omega and feedback
  mo: entity work.omega port map (sa(0), omega);
   
  f <= sa(31) xor sa(30) xor sa(26) xor sa(24) xor sa(19) xor sa(13) xor sa(12) xor sa(8) xor sa(6) xor omega;
 
  -- from left nonlinears
  x(36) <= f      xor i_s36; 
  x(29) <= sa(30) xor i_s29;
  x(23) <= sa(24) xor i_s23;
 
  -- from right nonlinears
  x(18) <= sa(19) xor i_s18;
  x(10) <= sa(11) xor i_s10;
  x(4)  <= sa(5)  xor i_s4;


  i_data_9 <= i_data(8) when load = '1' else i_data(9);  -- loading ( 64 bits instead of 70)

  -- absorbs 

  ab(36) <= sa(36) xor i_data_9;
  ab(35) <= sa(35) xor i_data(8);
  ab(34) <= sa(34) xor i_data(7);
  ab(27) <= sa(27) xor i_data(5);

  ab_27_17_8: for i in 0 to gf_dim -1  generate     
    ab(28)(i) <= sa(28)(i) xor ( not is_tag and i_data(6)(i));      -- also TAG --
    ab(16)(i) <= sa(16)(i) xor ( not is_tag and i_data(3)(i));      -- also TAG --
    ab(9)(i)  <= sa(9)(i)  xor ( not is_tag and i_data(1)(i));      -- also TAG --
  end generate ab_27_17_8;

  ab(18) <= sa(18) xor i_data(4);
  ab(15) <= sa(15) xor i_data(2);
  ab(8)  <= sa(8)  xor i_data(0);
  ab(0)(0 to 1)          <= sa(0)(0 to 1)  xor i_dom_sep(0 to 1);  
  ab(0)(2 to gf_dim - 1)   <= sa(0)(2 to gf_dim - 1);

-- absorbs muxes
  y(36) <= ab(36) when absorb = '1' else x(36);
  y(35) <= ab(35) when absorb = '1' else sa(36);
  y(34) <= ab(34) when absorb = '1' else sa(35);
  y(28) <= ab(28) when absorb = '1' else sa(29);
  y(27) <= ab(27) when absorb = '1' else sa(28);
  y(18) <= ab(18) when absorb = '1' else x(18);
  y(16) <= ab(16) when absorb = '1' else sa(17);
  y(15) <= ab(15) when absorb = '1' else sa(16);
  y(9)  <= ab(9)  when absorb = '1' else sa(10);
  y(8)  <= ab(8)  when absorb = '1' else sa(9);
  y(0)  <= ab(0)  when absorb = '1' else sa(1); -- dom sep 
 
  -- replace muxes and replace_or_load muxes 

  l(36)(0)             <= i_data_9(0)             when replace_or_load='1' else y(36)(0);             --RLmx36_bit0
  l(36)(1 to gf_dim-1) <= i_data_9(1 to gf_dim-1) when load='1'            else y(36)(1 to gf_dim-1); --Lmx36_others

  l(35) <= i_data(8) when replace = '1'         else y(35);  -- REPLACING 
  l(34) <= i_data(7) when replace = '1'         else y(34);  -- REPLACING 
  l(28) <= i_data(6) when replace = '1'         else y(28);  -- REPLACING 
  l(27) <= i_data(5) when replace_or_load = '1' else y(27);  -- LOADING/REPLACING 
  l(18) <= i_data(4) when replace_or_load = '1' else y(18);  -- LOADING/REPLACING 
  l(16) <= i_data(3) when replace_or_load = '1' else y(16);  -- LOADING/REPLACING 
  l(15) <= i_data(2) when replace = '1'         else y(15);  -- REPLACING 
  l(9)  <= i_data(1) when replace = '1'         else y(9);   -- REPLACING 
  l(8)  <= i_data(0) when replace_or_load = '1' else y(8);   -- LOADING/REPLACING  

  -- SB muxes
   l(29) <= sa(30) when sb_off = '1' else x(29);      -- LOADING/TAG turn off SB
   l(23) <= sa(24) when sb_off = '1' else x(23);      -- LOADING/TAG turn off SB
   l(10) <= sa(11) when sb_off = '1' else x(10);      -- LOADING/TAG turn off SB
   l(4)  <= sa(5)  when sb_off = '1' else x(4);       -- LOADING/TAG turn off SB


  -- lfsr update

  -- plain shifts ( do NOT update on absorb = '1')
    
  lfsr_shiftplain : for i in 0 to 35 generate  
    lfsr_if: if shiftplain(i) = '1' generate        
    lfsr_step: process(clk) begin
      if rising_edge(clk) then
      if lfsr_en = '1' then        
          sa(i) <= sa(i+1);
      end if;  
      end if;
    end process;
   end generate  lfsr_if;
  end generate  lfsr_shiftplain;


  -- stages updated from load muxes
  lfsr_load_shift: process(clk) begin
    if rising_edge(clk) then
      if lfsr_en = '1'  or absorb = '1' or replace = '1' then  
      sa(36) <= l(36);
      sa(35) <= l(35);
      sa(34) <= l(34);
      sa(28) <= l(28);
      sa(27) <= l(27);
      sa(18) <= l(18);
      sa(16) <= l(16);
      sa(15) <= l(15);
      sa(9)  <= l(9);
      sa(8)  <= l(8);
      if i_padding_reg /= '1' then sa(0)  <= y(0); end if;
      end if;
    end if;
  end process;


  -- stages updated from SB muxes
  lfsr_sbmx_shift: process(clk) begin
    if rising_edge(clk) then
      if lfsr_en = '1' then        
      sa(29) <= l(29);
      sa(23) <= l(23);
      sa(10) <= l(10);
      sa(4)  <= l(4);
      end if;
    end if;
  end process;

end;

