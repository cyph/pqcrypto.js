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

architecture rtl of ace is
  signal ctl_control    : ace_ctl_ty;
  signal ctl_onehot     : onehot_ty;
  signal ctl_lfsr_en    : std_logic;
  signal ctl_lfsr_reset : std_logic;
begin

  u_dp :
    entity work.dp port map
      ( clk       => clk
      , reset     => reset
      , i_mode    => i_mode
      , i_control => ctl_control
      , i_onehot  => ctl_onehot    
      , i_dom_sep => i_dom_sep
      , i_valid   => i_valid
      , i_data    => i_data
      , i_padding => i_padding
      , o_data    => o_data
     );

  u_ctl :
    entity work.ctl port map
      ( clk          => clk
      , reset        => reset
      , i_mode       => i_mode
      , i_dom_sep    => i_dom_sep
      , i_valid      => i_valid
      , i_padding    => i_padding
      , o_valid      => o_valid
      , o_onehot     => ctl_onehot
      , o_ready      => o_ready
      , o_control    => ctl_control
     );

end architecture;  

  
    
