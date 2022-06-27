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

------------------------------------------
--- sbox for WAGE: last two NOTs
------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

entity not_sb is 
  port
    ( i_a : in  std_logic_vector(0 to 6)
    ; o_z : out std_logic_vector(0 to 6)
   ); 
end not_sb; 

architecture rtl of not_sb is 
begin

  o_z(0) <= not i_a(0);
  o_z(1) <= i_a(1);
  o_z(2) <= not i_a(2);
  o_z(3) <= i_a(3);  
  o_z(4) <= i_a(4);
  o_z(5) <= i_a(5);
  o_z(6) <= i_a(6);

end;  
