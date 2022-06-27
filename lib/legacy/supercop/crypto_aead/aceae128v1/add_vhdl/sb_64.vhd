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
use work.ace_pkg.all;

entity sb_64 is
  port
    ( i_state : in  word
    ; i_rc    : in  std_logic
    ; o_state : out word
    );
end entity;

architecture rtl of sb_64 is
  signal x0, x1, z0, z1 : half_word;
  signal rc : half_word;
begin

  x1 <= i_state(            0 to half_word_sz-1 );
  x0 <= i_state( half_word_sz to word_sz - 1    );

  rc <= ( 0 to half_word_sz - 2 => '1', half_word_sz - 1 => i_rc ); 

  z0 <= x1;
  z1 <=    ( ( x1(5 to half_word_sz - 1) & x1 (0 to 4) ) and x1)
        xor (  x1(1 to half_word_sz - 1) & x1 (0)      )
        xor x0
        xor rc;
          
          
  o_state <= z1 & z0;

end architecture;
