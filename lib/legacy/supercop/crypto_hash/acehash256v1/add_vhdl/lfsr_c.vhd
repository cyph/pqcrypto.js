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
use work.ace_pkg.all;


entity lfsr_c is
  port
    ( clk          : in std_logic
    ; lfsr_c_en    : in std_logic
    ; lfsr_c_reset : in std_logic
    ; o_const      : out lfsr_c_output
   );
end lfsr_c;

architecture rtl of lfsr_c is
  signal sa: std_logic_vector(lfsr_c_sz - 1 downto 0);
  signal xa: std_logic_vector(lfsr_c_sz + 2 downto 0);
begin

-- 10 output bits for the constants
  o_const <= xa;  -- "to" type <= "downto" type. Index flip intended

-- just rename signal
  xa(lfsr_c_sz-1 downto 0) <= sa(lfsr_c_sz-1 downto 0);

-- for updates and outputs 
  xa(lfsr_c_sz + 2 downto lfsr_c_sz) <= xa(3 downto 1) xor xa(2 downto 0);

  lfsr_shift: for i in lfsr_c_sz-1 downto 0 generate
    lfsr_step: process(clk) begin
      if rising_edge(clk) then
        if lfsr_c_reset ='1' then
          sa(i) <= '1'; 
        elsif lfsr_c_en ='1' then
          sa(i) <= xa(i+3);  
        end if;
      end if;
    end process;
  end generate lfsr_shift;

end;

