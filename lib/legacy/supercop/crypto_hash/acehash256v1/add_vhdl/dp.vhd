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
  
entity dp is
  port
    ( clk       : in  std_logic
    ; reset     : in  std_logic
    ; i_mode    : in  mode_ty
    ; i_control : in  ace_ctl_ty
    ; i_onehot  : in  onehot_ty
    ; i_dom_sep : in  domsep_ty
    ; i_valid   : in  std_logic
    ; i_data    : in  word
    ; i_padding : in  std_logic
    ; o_data    : out word
   );
end entity;

architecture rtl of dp is

  signal permoff, endstep, squeeze,
         absorb, replace, output,
         lfsr_c_reset, lfsr_c_en      : std_logic;
  signal ace_state, post_input        : half_word_state_ty;
  signal pre_round, post_round,
         post_xor, post_step_const,
         post_linear, ace_path        : word_state_ty;
  signal dsxor                        : half_word;
  signal i_data_vector, o_data_vector : half_word_data;
  signal ctl_const                    : lfsr_c_output;
begin

  u_lfsr_c :
    entity work.lfsr_c port map
      (	clk          => clk
      , lfsr_c_reset => lfsr_c_reset
      , lfsr_c_en    => lfsr_c_en
      , o_const      => ctl_const
     );

  i_data_vector <= data_to_vector( i_data );
  o_data        <= vector_to_data( o_data_vector );

  absorb        <= i_control( absorb_idx );
  replace       <= i_control( replace_idx );
  output        <= i_control( output_idx );
  endstep       <= i_control( endstep_idx );
  permoff       <= i_control( permoff_idx );
  squeeze       <= i_control( squeeze_idx );
  lfsr_c_reset  <= i_control( lfsr_c_reset_idx );
  lfsr_c_en     <= i_control( lfsr_c_en_idx );

  ----------------------------------------------------------------------
  -- post input:  do input and domain separator and replace
  
  post_input( a1_idx ) <= i_data_vector(0) xor ace_state( a1_idx ) when absorb = '1' and replace = '0'  and i_valid = '1'
                     else i_data_vector(0)                         when absorb = '1' and replace = '1'  and i_valid = '1'
                     else ace_state( a1_idx );
          
  post_input( c1_idx ) <= i_data_vector(1) xor ace_state( c1_idx ) when absorb = '1' and replace = '0'  and i_valid = '1'
                     else i_data_vector(1)                         when absorb = '1' and replace = '1'  and i_valid = '1'
                     else ace_state( c1_idx );                
                                
  post_input( a0_idx ) <= ace_state( a0_idx );
  post_input( c0_idx ) <= ace_state( c0_idx );        
  post_input( b0_idx ) <= ace_state( b0_idx );        
  post_input( b1_idx ) <= ace_state( b1_idx );        
  post_input( d0_idx ) <= ace_state( d0_idx );
  post_input( d1_idx ) <= ace_state( d1_idx );
  post_input( e1_idx ) <= ace_state( e1_idx );        
        
  dsxor( 0 to half_word_sz - 3 ) <= ( others => '0' );
  dsxor( half_word_sz - 2)       <= i_dom_sep(1);  
  dsxor( half_word_sz - 1)       <= i_dom_sep(0);
  
  post_input( e0_idx ) <= dsxor xor ace_state( e0_idx ) when (i_valid = '1')
                     else ace_state( e0_idx );        
                                
  ----------------------------------------------------------------------
  --  !!! MDA update pic with output MUXes 
           
  o_data_vector(0) <= i_data_vector(0) xor ace_state( a1_idx ) when output = '1' and absorb = '1' and i_valid = '1'
                 else ace_state( a1_idx )                      when output = '1' and ( i_onehot(2) = '1' or squeeze = '1' ) -- first tag or squeeze
                 else ace_state( c1_idx )                      when output = '1'                                            -- second tag
                 else ( others => '0' );

  o_data_vector(1) <= i_data_vector(1) xor ace_state( c1_idx ) when output = '1' and absorb = '1' and i_valid = '1'
                 else ace_state( c1_idx )                      when output = '1' and squeeze = '1'                  -- squeeze
                 else ace_state( a0_idx )                      when output = '1' and i_onehot(2) = '1'              -- first tag
                 else ace_state( c0_idx )                      when output = '1'                                    -- second tag
                 else ( others => '0' );        
                                
  ---------------------------------------------------------------------
  -- sb 64 ==> post round  

  pre_round <= half_words_to_words( post_input );

  a_sb_64 :
    entity work.sb_64 port map
      ( i_state => pre_round( a_idx )
      , i_rc    => ctl_const( lfsr_c_sz + 2 )   --rc0
      , o_state => post_round( a_idx )
     );
  
  post_round( b_idx ) <= pre_round( b_idx );
  
  c_sb_64 :
    entity work.sb_64 port map
      ( i_state => pre_round( c_idx )
      , i_rc    => ctl_const( lfsr_c_sz + 1 )   --rc1
      , o_state => post_round( c_idx )
     );
  
  post_round( d_idx ) <= pre_round( d_idx );
  
  e_sb_64 :
    entity work.sb_64 port map
      ( i_state => pre_round( e_idx )
      , i_rc    => ctl_const( lfsr_c_sz )       -- rc2
      , o_state => post_round( e_idx )
     );
  
  ----------------------------------------------------------------------
  -- XORs to the left ==> post xor 

  post_xor( a_idx ) <= post_round( a_idx );
  post_xor( c_idx ) <= post_round( c_idx );  

  post_xor( b_idx ) <= post_round( b_idx ) xor post_round( c_idx );
  post_xor( d_idx ) <= post_round( d_idx ) xor post_round( e_idx );
  post_xor( e_idx ) <= post_round( e_idx ) xor post_round( a_idx );
               
  ----------------------------------------------------------------------
  -- XOR with step constant ==> post step const
    
  post_step_const( a_idx )           <= post_xor( a_idx );
  post_step_const( c_idx )           <= post_xor( c_idx );  
  
  post_step_const( b_idx)( 0 to 55)  <= not post_xor( b_idx )( 0 to 55  );                                
  post_step_const( b_idx)( 56 to 63) <= post_xor(     b_idx )( 56 to 63 ) xor ctl_const( 2 to lfsr_c_sz + 2 ); -- sc0

  post_step_const( d_idx)( 0 to 55)  <= not post_xor( d_idx )( 0 to 55  );
  post_step_const( d_idx)( 56 to 63) <= post_xor(     d_idx )( 56 to 63 ) xor ctl_const( 1 to lfsr_c_sz + 1 ); -- sc1

  post_step_const( e_idx)( 0 to 55)  <= not post_xor( e_idx )( 0 to 55  );
  post_step_const( e_idx)( 56 to 63) <= post_xor(     e_idx )( 56 to 63 ) xor ctl_const( 0 to lfsr_c_sz );     --sc2

  ------------------------------------------------------------
  -- post liear layer pi = (3,2,0,4,1) ==> post linear
    
  post_linear( a_idx ) <= post_step_const( d_idx );
  post_linear( b_idx ) <= post_step_const( c_idx );
  post_linear( c_idx ) <= post_step_const( a_idx );
  post_linear( d_idx ) <= post_step_const( e_idx );
  post_linear( e_idx ) <= post_step_const( b_idx );

  ------------------------------------------------------------
  -- update state
        
  ace_path( a_idx ) <= post_linear( a_idx ) when endstep = '1'  else post_round( a_idx );
  ace_path( b_idx ) <= post_linear( b_idx ) when endstep = '1'  else post_round( b_idx );
  ace_path( c_idx ) <= post_linear( c_idx ) when endstep = '1'  else post_round( c_idx );
  ace_path( d_idx ) <= post_linear( d_idx ) when endstep = '1'  else post_round( d_idx );
  ace_path( e_idx ) <= post_linear( e_idx ) when endstep = '1'  else post_round( e_idx );

  process
  begin

    wait until rising_edge(clk);
    -- ACE_AE
    if i_mode(1) = '0' then 
      if permoff = '0' then         
        ace_state <= words_to_half_words( ace_path ); -- every clk cycle
      else
        if i_valid = '1' and i_padding = '0' then     -- load
          if i_onehot(0) = '1' then 
            ace_state( a0_idx ) <= i_data_vector(1);  -- k0
            ace_state( a1_idx ) <= i_data_vector(0);
          end if; 
          if i_onehot(1) = '1' then         
            ace_state( c0_idx ) <= i_data_vector(1);  -- k1
            ace_state( c1_idx ) <= i_data_vector(0);
          end if;                         
          if i_onehot(2) = '1' then 
            ace_state( b0_idx ) <= i_data_vector(1);  -- n0
            ace_state( b1_idx ) <= i_data_vector(0);
          end if;                                 
          if i_onehot(3) = '1' then 
            ace_state( e0_idx ) <= i_data_vector(1);  -- n1
            ace_state( e1_idx ) <= i_data_vector(0);
            ace_state( d0_idx ) <= ( others => '0' );
            ace_state( d1_idx ) <= ( others => '0' );        
          end if;                                 
        end if;
      end if;
    -- ACE_H        
    else 
      if permoff = '0' then         
        ace_state <= words_to_half_words( ace_path );
      else 
        if i_valid = '1' then                        -- load
          ace_state( b0_idx ) <= i_data_vector(1);
          ace_state( b1_idx ) <= i_data_vector(0);
          ace_state( a0_idx ) <= ( others => '0' );
          ace_state( a1_idx ) <= ( others => '0' );        
          ace_state( c0_idx ) <= ( others => '0' );
          ace_state( c1_idx ) <= ( others => '0' );                                                
          ace_state( d0_idx ) <= ( others => '0' );
          ace_state( d1_idx ) <= ( others => '0' );        
          ace_state( e0_idx ) <= ( others => '0' );
          ace_state( e1_idx ) <= ( others => '0' );                                
        end if;
      end if;                        
    end if;

  end process;

end architecture;
