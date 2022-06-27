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
  
entity ctl is
  port
    ( clk       : in  std_logic
    ; reset     : in  std_logic
    ; i_mode    : in  mode_ty
    ; i_dom_sep : in  domsep_ty
    ; i_valid   : in  std_logic
    ; i_padding : in  std_logic
    ; o_valid   : out std_logic
    ; o_onehot  : out onehot_ty
    ; o_ready   : out std_logic
    ; o_control : out ace_ctl_ty
   );
end entity;

architecture rtl of ctl is

  type state_ty is (Load, pLoad, iLoad2, iInit, pInit, iProcAD, pProcAD, pEncrypt, pDecrypt, iFinal, pFinal, Tag, iHash, pAbsorb, pSqueeze);
  signal state, state_next   : state_ty;
  signal count, count_next   : count_ty;
  signal onehot, onehot_next : onehot_ty;
  signal permoff, lfsr_c_reset : std_logic;
  constant onehot_reset      : onehot_ty := ( 0 => '1', others => '0' ); --    0001
  constant count_reset       : count_ty  := ( 7 => '1', others => '0' ); -- 1000000
  constant count_zero        : count_ty  := ( others => '0' );           -- 0000000
  constant count_one         : count_ty  := ( 0 => '1', others => '0' ); -- 0000001

begin

---------------- STATE UPDATE ----------------

  o_control ( lfsr_c_reset_idx ) <= lfsr_c_reset;

  lfsr_c_reset   <= '1' when reset = '1' or (count(7) = '0' and count_next(7) = '1' ) --- reset after last ACE permutation
             else '0';

  o_control ( lfsr_c_en_idx )    <= not( permoff ) or lfsr_c_reset;
  o_onehot     <= onehot;

  o_ready      <= '1' when count(7) = '1' and state /= Tag and state_next /= Tag and state /= pSqueeze and state_next /= pSqueeze
             else '0';

  o_valid      <= '1' when ( i_valid = '1' and count(7) = '1' and 
                           ( state_next = pEncrypt or state_next = pDecrypt or state_next = pSqueeze ) )
                            or
                           ( count(7) = '1' and ( state = pSqueeze or state_next = pSqueeze ) )
                            or
                           ( state = Tag or state_next = Tag )
             else '0';

  o_control( absorb_idx )  <= '1' when state_next = pAbsorb or state_next = pEncrypt or state_next = pFinal or
                                       state_next = pDecrypt or state_next = pProcAD or state_next = pInit
                         else '0';

  o_control( replace_idx ) <= '1' when state_next = pDecrypt 
                         else '0';

  o_control( output_idx )  <= '1' when state_next = pEncrypt or state_next = pDecrypt or state = pSqueeze or
                                       state_next = pSqueeze or state = Tag or state_next = Tag
                         else '0';

  o_control( squeeze_idx ) <= '1' when state_next = pSqueeze or state = pSqueeze
                         else '0';

  permoff                  <= '0' when ( count(7) = '0' or ( count(7) = '1' and count_next(0) = '1' ) )
                                       and i_padding = '0'
                         else '1';

  o_control( permoff_idx ) <= permoff;        
  o_control( endstep_idx ) <= count(0) and count(1) and count(2);

  process begin

    wait until rising_edge(clk);
    if reset = '1' then
      state  <= Load;
      onehot <= onehot_reset;
      count  <= count_reset;
    else
      state  <= state_next;
      onehot <= onehot_next;
      count  <= count_next;
    end if;

  end process;

  ------------- NEXT STATE LOGIC ----------------

  process(state, onehot, count, i_valid, i_mode, i_dom_sep, i_padding) is
  begin

    case state is
      when Load =>
        if i_valid = '1' then
          if onehot(3) = '1' or i_mode(1) = '1' then       -- if onehot = 1000 or if HASH mode
            state_next  <= pLoad;
            count_next  <= count_zero;                     -- count goes from 128 to 0
            onehot_next <= onehot_reset;
          else
            state_next  <= Load;
            count_next  <= count_reset;                    -- keep count at 128
            onehot_next <= onehot_rotate( onehot );        -- rotate onehot
          end if;
        else
          count_next  <= count_reset;                      -- keep count at 128
          state_next  <= Load;
          onehot_next <= onehot;                           -- keep onehot unchanged 
        end if;

      when pLoad | iLoad2 =>
        if count < 128 and state = pLoad then
          count_next  <= count + 1;                        -- increment count if less than 128
          state_next  <= pLoad;
          onehot_next <= onehot_reset;                     -- keep onehot at 0001
        elsif i_mode(0) = '1' and i_mode(1) = '1' then
          state_next  <= pSqueeze;
          onehot_next <= onehot_reset;
          count_next  <= count_one;
        elsif i_valid = '1' then
          count_next <= count_one;                         -- count goes from 128 to 1
          if i_mode(1) = '0' then                          -- and i_dom_sep(0) = '0' and i_dom_sep(1) = '0' then
            onehot_next <= onehot_rotate( onehot );        -- rotate onehot from 0001 to 0010
            state_next  <= pInit;
          else                                             -- i_mode(1) = '1' and i_mode(0) = '0' -- and i_dom_sep(0) = '0' and i_dom_sep(1) = '0' then
            onehot_next <= onehot_reset;                   -- keep onehot 0001
            state_next  <= pAbsorb;                
        end if;
      else
        count_next  <= count_reset;                              -- keep count at 128
        state_next  <= iLoad2;
        onehot_next <= onehot_reset;                       -- keep onehot at 0001
      end if;

    when pInit =>
      if count < 128 then
        count_next  <= count + 1;                          -- increment count if less than 128
        state_next  <= pInit;
        onehot_next <= onehot;                             -- keep onehot unchanged
      else
        if onehot(1) = '1' then                            -- if onehot = 0010 (i.e. K0 init is done, K1 is being sampled)
          if i_valid = '1' then
            onehot_next <= onehot_rotate( onehot );        -- rotate onehot from 0010 to 0100
            state_next  <= pInit;
            count_next  <= count_one;                      -- count goes from 128 to 1
          else
            onehot_next <= onehot;                         -- keep onehot unchanged (0010)
            state_next  <= iInit;
            count_next  <= count_reset;                    -- keep count at 128
          end if;
        else                                               -- if onehot = 0100
          onehot_next <= onehot_reset;                     -- reset onehot from 0100 to 0001
          if i_valid = '1' then
            if i_dom_sep(0) = '0' and i_dom_sep(1) = '0' then        -- i_dom_sep = 0
              state_next <= pFinal;
              count_next <= count_one;                     -- count goes from 128 to 1
            elsif i_dom_sep(0) = '1' and i_dom_sep(1) = '0' then     -- i_dom_sep = 1
              state_next <= pProcAD;
              count_next <= count_one;                     -- count goes from 128 to 1
            elsif i_dom_sep(0) = '0' and i_dom_sep(1) = '1' then     -- i_dom_sep = 2
              if i_padding = '0' then
                count_next <= count_one;                   -- count goes from 128 to 1
              else
                count_next <= count_zero;                  -- count goes from 128 to 0 when padding is needed
              end if;
              if i_mode(0) = '0' then
                state_next <= pEncrypt;
              else                                         -- i.e. if i_mode(0) = '1'
                state_next <= pDecrypt;
               end if;
            else                                           -- if i_valid = 1 but i_dom_sep has ILLEGAL value = 3
              state_next <= iProcAD;
              count_next <= count_reset;                   -- keep count at 128
            end if;
          else                                             -- i.e. if i_valid = '0'
            state_next <= iProcAD;
            count_next <= count_reset;                     -- keep count at 128
          end if;
        end if;
      end if;

    when iInit =>
      if i_valid = '1' then
        onehot_next <= onehot_rotate( onehot );            -- rotate onehot from 0010 to 0100
        state_next  <= pInit;
        count_next  <= count_one;                          -- count goes from 128 to 1
      else
        onehot_next <= onehot;                             -- keep onehot at 0010 (staying ready for K1)
        state_next  <= iInit;
        count_next  <= count_reset;                        -- keep count at 128
      end if;

    when iProcAD | pProcAD | pEncrypt | pDecrypt =>
      if count < 128 and state /= iProcAD then
        if i_padding = '0' then
          count_next <= count + 1;                         -- increment count if less than 128 (except if in iProcAD)
        else
          count_next <= count;                             -- stall count when i_padding = 1
        end if;                
        state_next  <= state;
        onehot_next <= onehot_reset;                       -- keep onehot at 0001
      else
        if i_valid = '1' then
          if i_dom_sep(0) = '0' and i_dom_sep(1) = '0' then          -- i_dom_sep = 0
            onehot_next <= onehot_rotate(onehot);          -- rotate onehot to 0010
            state_next  <= pFinal;
            count_next  <= count_one;                      -- count goes from 128 to 1
          elsif i_dom_sep(0) = '1' and i_dom_sep(1) = '0' then       -- i_dom_sep = 1
            state_next  <= pProcAD;
            count_next  <= count_one;                      -- count goes from 128 to 1
            onehot_next <= onehot_reset;                   -- keep onehot at 0001
          elsif i_dom_sep(0) = '0' and i_dom_sep(1) = '1' then       -- i_dom_sep = 2
            if i_padding = '0' then
              count_next <= count_one;                     -- count goes from 128 to 1
            else
              count_next <= count_zero;                    -- count goes from 128 to 0 when padding is needed
            end if;
            onehot_next <= onehot_reset;                   -- keep onehot at 0001
            if i_mode(0) = '0' then
              state_next <= pEncrypt;
            else                                           -- i.e. if i_mode(0) = '1'
              state_next <= pDecrypt;
            end if;
          else                                             -- if i_valid = 1 but i_dom_sep has ILLEGAL value = 3
            state_next <= iProcAD;
            count_next <= count_reset;                     -- keep count at 128
            onehot_next <= onehot;
          end if;        
        else                                               -- i.e. if i_valid = '0'
          state_next <= iProcAD;
          count_next <= count_reset;                       -- keep count at 128
          onehot_next <= onehot;
        end if;
     end if;

    when pFinal =>
      if count < 128 then
        count_next  <= count + 1;                          -- increment count if less than 128 (except if in iProcAD)
        state_next  <= pFinal;
        onehot_next <= onehot;                             -- keep onehot constant (0010 or 0100)
      else        
         if onehot(1) = '1' then                           -- if onehot = 0010
          if i_valid = '1' then
            onehot_next <= onehot_rotate( onehot );        -- rotate onehot from 0010 to 0100
            state_next  <= pFinal;
            count_next  <= count_one;                      -- count goes from 128 to 1
          else
            onehot_next <= onehot;                         -- keep onehot unchanged (at 0010)
            state_next  <= iFinal;
            count_next  <= count_reset;                    -- keep count at 128
          end if;
        else                                               -- if onehot = 0100
          state_next  <= Tag;
          onehot_next <= onehot_rotate( onehot );          -- rotate onehot from 0100 to 1000
          count_next  <= count_reset;                      -- keep count at 128
        end if;
      end if;

    when iFinal =>
      if i_valid = '1' then
        onehot_next <= onehot_rotate( onehot );            -- rotate onehot from 0010 to 0100
        state_next  <= pFinal;
        count_next  <= count_one;                          -- count goes from 128 to 1
      else
        onehot_next <= onehot;                             -- keep onehot at 0010
        state_next  <= iFinal;
        count_next  <= count_reset;                        -- keep count at 128
      end if;

    when Tag =>
        count_next  <= count_reset;                        -- keep count at 128
        onehot_next <= onehot_rotate( onehot );
        state_next  <= Load;

    when iHash | pAbsorb =>
      if count < 128 and state = pAbsorb then
        count_next  <= count + 1;                          -- increment count if less than 128
        state_next  <= pAbsorb;
        onehot_next <= onehot_reset;                       -- keep onehot 0001
      elsif i_mode(0) = '1' and i_mode(1) = '1' then
        state_next  <= pSqueeze;
        onehot_next <= onehot_reset;
        count_next  <= count_one;
      elsif i_valid = '1' then
        count_next  <= count_one;                          -- count goes from 128 to 1
        state_next  <= pAbsorb;
        onehot_next <= onehot_reset;                       -- onehot stays at 0001
      else
        count_next  <= count_reset;                        -- keep count at 128
        onehot_next <= onehot;                             -- onehot remains unchanged
        state_next  <= iHash;        
      end if;
        
    when pSqueeze =>
      if count < 128 then
        count_next  <= count + 1;                          -- increment count if less than 128
        state_next  <= pSqueeze;
        onehot_next <= onehot;                             -- keep onehot
      else
        if onehot(2) = '1' then
          onehot_next <= onehot_reset;                        
          state_next  <= Load;
          count_next  <= count_reset;                      -- keep count at 128
        else
          onehot_next <= onehot_rotate( onehot );          -- onehot goes from 0010 to 0100 for second squeeze
          state_next  <= pSqueeze;
          count_next  <= count_one;                        -- count goes from 128 to 1
        end if;
      end if;

    when others =>
      state_next  <= Load;
      onehot_next <= onehot_reset;
      count_next  <= count_reset;

  end case;

  end process;

end architecture;  
