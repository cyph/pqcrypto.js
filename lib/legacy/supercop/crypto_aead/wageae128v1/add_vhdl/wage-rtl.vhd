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

architecture rtl of wage is
  signal ctl_control   : wage_ctl_ty;
  signal local_i_data,
         local_o_data  : data_io;
begin

  input: for i in 0 to 8 generate
    local_i_data(i) <= i_data( i*gf_dim to (i+1)*gf_dim-1 );
  end generate;

  local_i_data(9) <= (0 => i_data(63), others => '0');

  output: for i in 0 to 8 generate
    o_data( i*gf_dim to (i+1)*gf_dim-1 ) <= local_o_data(i);
  end generate;

  o_data(63) <= local_o_data(9)(0);

  -------------------------------------------------------------

  u_dp : entity work.dp port map 
    ( clk       => clk
    , i_control => ctl_control
    , i_dom_sep => i_dom_sep
    , i_data    => local_i_data
    , o_data    => local_o_data
   );

  ------------------------------------------------------------
  u_ctl : entity work.ctl port map
    ( clk       => clk
    , reset     => reset
    , i_mode    => i_mode
    , i_dom_sep => i_dom_sep
    , i_valid   => i_valid
    , i_padding => i_padding
    , o_valid   => o_valid
    , o_ready   => o_ready
    , o_control => ctl_control
   );

end architecture;  

  
    
