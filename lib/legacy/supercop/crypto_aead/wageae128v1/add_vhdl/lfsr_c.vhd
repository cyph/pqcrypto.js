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

entity lfsr_c is  
  port
    ( clk,
      lfsr_c_en,
      lfsr_c_reset    : in std_logic
    ; o_const  : out std_logic_vector(lfsr_c_sz downto 0)
   );
end lfsr_c;

architecture rtl of lfsr_c is
  signal f:  std_logic;
  signal sa: state_array_c;
  signal xa: std_logic_vector( 0 to lfsr_c_sz+1 );
begin

  -- both constants
  outputs1: for i in 0 to lfsr_c_sz generate
    o_const(i) <= xa(i);
  end generate outputs1;

  x_sa: for i in 0 to lfsr_c_sz - 1 generate
    xa(i) <= sa(i);
  end generate x_sa;

  -- for updates and outputs 
  x_values: for i in lfsr_c_sz to lfsr_c_sz + 1 generate
    xa(i) <= xa( i-(lfsr_c_sz-1) ) xor xa( i-lfsr_c_sz );
  end generate x_values;

  -- lfsr update
  lfsr_shift: for i in 0 to lfsr_c_sz - 1 generate
    lfsr_step: process(clk) begin
      if rising_edge(clk) then
        if lfsr_c_en = '1' then
          sa(i) <= xa(i+2) or lfsr_c_reset;
        end if;
      end if;
    end process;
  end generate lfsr_shift;

end;

