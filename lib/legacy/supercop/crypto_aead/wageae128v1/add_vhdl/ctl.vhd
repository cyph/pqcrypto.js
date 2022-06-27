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
use work.wage_pkg.all;
  
entity ctl is
  port
    ( clk
    , reset         : in  std_logic
    ; i_mode        : in  mode_ty
    ; i_dom_sep     : in  domsep_ty
    ; i_valid       : in  std_logic
    ; i_padding     : in  std_logic
    ; o_valid       : out std_logic
    ; o_ready       : out std_logic
    ; o_control     : out wage_ctl_ty
   );
end entity;

architecture rtl of ctl is
  type   state_ty is (sLoad, pLoad, iLoad2, iInit, pInit, iProcAD, pProcAD, pEncrypt, pDecrypt, iFinal, pFinal, sTag);
  signal state, state_next : state_ty;
  signal count, count_next : count_ty;

  signal lfsr_c_reset : std_logic;
  signal k_counter, k_counter_next : std_logic;

  constant count_reset   : count_ty  := ( others => '0');          -- 0000000
  constant count_zero    : count_ty  := ( others => '0');          -- 0000000
  constant count_one     : count_ty  := (0 => '1', others => '0'); -- 0000001
  constant count_load    : count_ty  := to_unsigned(8, bits_counter);
  constant rounds        : natural := 111;
  signal   i_padding_reg : std_logic;
  signal   int_o_ready   : std_logic;
begin

  o_control( i_padding_reg_idx ) <= i_padding_reg;

  o_control( absorb_idx )  <= '1' when (int_o_ready = '1' and i_valid = '1' and state /= sLoad)
                         else '0';

  o_control( replace_idx ) <= '1' when (int_o_ready = '1' and i_valid = '1' and state_next = pDecrypt)
                         else '0';

  o_control( load_idx)     <= '1' when (state = sLoad)
                         else '0';  

  o_control( sb_off_idx)   <= '1' when state = sLoad or state = sTag or state_next = sTag
                         else '0';

  o_control( is_tag_idx)   <= '1' when state = sTag or state_next = sTag
                         else '0';

  o_control( lfsr_c_reset_idx ) <= lfsr_c_reset or reset;

  lfsr_c_reset <= '1' when count = (rounds - 1) or (state = sLoad and state_next = pLoad)
             else '0';

  o_control ( lfsr_en_idx) <= '0' when (
                                         (
                                           (int_o_ready = '1' and state /= sLoad and i_padding_reg /= '1')
                                           or
                                           (state = sLoad and i_valid = '0')
                                         )
                                         and lfsr_c_reset = '0' and reset = '0'
                                       )
                                       or i_padding_reg = '1' or i_padding = '1'
                         else '1';


  o_valid <= '1' when (i_valid = '1' and count = rounds and (state_next = pEncrypt or state_next = pDecrypt))
                      or
                      state_next = sTag
        else '0';

  int_o_ready <= '1' when (count = rounds and (state /= pFinal or k_counter = '0'))
                          or state = sLoad or (state = iLoad2) or i_padding = '1' or i_padding_reg = '1'
            else '0';

  o_ready <= int_o_ready;

  ---------------- STATE UPDATE ----------------

  process begin
    wait until rising_edge(clk);
      i_padding_reg <= i_padding;
  end process;

  process begin
    wait until rising_edge(clk);
      if reset = '1' then
        state  <= sLoad;
        count  <= count_reset;
        k_counter <= '0';
      else
        state  <= state_next;
        count  <= count_next;
        k_counter <= k_counter_next;
      end if;
  end process;

------------- NEXT STATE LOGIC ----------------

  process(state, count, i_valid, i_mode, i_dom_sep, i_padding, i_padding_reg, k_counter) is
  begin

    case state is
      when sLoad =>
        if i_valid = '1' then
          if count = count_load then
            state_next <= pLoad;
            count_next <= count_zero;      
            k_counter_next <= '0';
          else
            state_next <= sLoad;
            count_next <= count + 1;  
            k_counter_next <= '0';
          end if;
        else
          count_next  <= count;  -- keep count unchanged
          state_next  <= sLoad;
          k_counter_next <= '0';
        end if;

      when pLoad | iLoad2 =>
        if count < rounds and state = pLoad then
          count_next  <= count + 1;      -- increment count if less than 128
          state_next  <= pLoad;
          k_counter_next <= '0';
        elsif i_valid = '1' then
          count_next <= count_zero;      -- count goes from 128 to 1
          state_next  <= pInit;
          k_counter_next <= '0';
        else
          count_next  <= count;  -- keep count at 128
          state_next  <= iLoad2;
          k_counter_next <= '0';
        end if;
  
      when pInit =>
        if count < rounds then
          count_next  <= count + 1;      -- increment count if less than 128
          state_next  <= pInit;
          k_counter_next <= k_counter;
        else
          if k_counter = '0' then  -- if onehot = 0010 (i.e. K0 init is done, K1 is being sampled)
            if i_valid = '1' then
              k_counter_next <= '1';  -- rotate onehot from 0010 to 0100
              state_next  <= pInit;
              count_next  <= count_zero;      -- count goes from 128 to 0
            else
              k_counter_next <= '0';
              state_next  <= iInit;
              count_next  <= count;      -- keep count at 128
            end if;
          else      -- if onehot = 0100
            k_counter_next <= '0';
            if i_valid = '1' then
              if i_dom_sep(0) = '0' and i_dom_sep(1) = '0' then  -- i_dom_sep = 0
                state_next <= pFinal;
                count_next <= count_zero;      -- count goes from 128 to 0
              elsif i_dom_sep(0) = '1' and i_dom_sep(1) = '0' then  -- i_dom_sep = 1
                state_next <= pProcAD;
                count_next <= count_zero;      -- count goes from 128 to 0
              elsif i_dom_sep(0) = '0' and i_dom_sep(1) = '1' then  -- i_dom_sep = 2
                count_next <= count_zero;    -- count goes from 128 to 0
    
                if i_mode = encrypt_mode then
                  state_next <= pEncrypt;
                else     -- i.e. if i_mode(0) = '1'
                  state_next <= pDecrypt;
                end if;
    
              else    -- if i_valid = 1 but i_dom_sep has ILLEGAL value = 3 (GOOD FOR PROTECTED?)
                state_next <= iProcAD;
                count_next <= count;      -- keep count at 128
              end if;
            else       -- i.e. if i_valid = '0'
              state_next <= iProcAD;
              count_next <= count;      -- keep count at 128
            end if;
          end if;
        end if;
  
      when iInit =>
        if i_valid = '1' then
          k_counter_next <= '1';
          state_next  <= pInit;
          count_next  <= count_zero;      -- count goes from 128 to 1
        else
          k_counter_next <= '0';
          state_next  <= iInit;
          count_next  <= count;  -- keep count at 128
        end if;

      when iProcAD | pProcAD | pEncrypt | pDecrypt =>
        if count < rounds and state /= iProcAD then
          if i_padding = '0' and i_padding_reg = '0' then
            count_next <= count + 1;      -- increment count if less than 128 (except if in iProcAD)
          else
            count_next <= count;  -- stall count when i_padding = 1
          end if;    

          state_next  <= state;
          k_counter_next <= '0';
        else
          if i_valid = '1' then
            if i_dom_sep(0) = '0' and i_dom_sep(1) = '0' then  -- i_dom_sep = 0
              k_counter_next <= '0';
              state_next  <= pFinal;
              count_next  <= count_zero;      -- count goes from 128 to 1
            elsif i_dom_sep(0) = '1' and i_dom_sep(1) = '0' then  -- i_dom_sep = 1
              state_next  <= pProcAD;
              count_next  <= count_zero;      -- count goes from 128 to 1
              k_counter_next <= '0';
            elsif i_dom_sep(0) = '0' and i_dom_sep(1) = '1' then  -- i_dom_sep = 2
              count_next <= count_zero;    -- count goes from 128 to 0
              k_counter_next <= '0';

              if i_mode = encrypt_mode then
                state_next <= pEncrypt;
              else     -- i.e. if i_mode(0) = '1'
                state_next <= pDecrypt;
              end if;

            else      -- if i_valid = 1 but i_dom_sep has ILLEGAL value = 3
              state_next <= iProcAD;
              count_next <= count;      -- keep count at 128
              k_counter_next <= '0';
            end if;  

          else       -- i.e. if i_valid = '0'
            state_next <= iProcAD;
            count_next <= count;  -- keep count at 128
            k_counter_next <= '0';
          end if;
        end if;

      when pFinal =>
        if count < rounds then
          count_next  <= count + 1;      -- increment count if less than 128 (except if in iProcAD)
          state_next  <= pFinal;
          k_counter_next <= k_counter;     
        else  
          if k_counter = '0' then  -- if onehot = 0010
            if i_valid = '1' then
              k_counter_next <= '1';      -- rotate onehot from 0010 to 0100
              state_next  <= pFinal;
              count_next  <= count_zero;      -- count goes from 128 to 1
            else
              k_counter_next <= '0';
              state_next  <= iFinal;
              count_next  <= count;      -- keep count at 128
            end if;
          else      -- if onehot = 0100
            state_next  <= sTag;
            k_counter_next <= '0';
            count_next  <= count_zero;      -- keep count at 128
          end if;
        end if;
  
      when iFinal =>
        if i_valid = '1' then
          k_counter_next <= '1';
          state_next  <= pFinal;
          count_next  <= count_zero;      -- count goes from 128 to 1
        else
          k_counter_next <= '0';
          state_next  <= iFinal;
          count_next  <= count;  -- keep count at 128
        end if;
  
      when sTag =>
        if count = count_load then
          count_next  <= count_zero;      -- 
          k_counter_next <= '0';
          state_next  <= sLoad;  -- TEMP turn OFF, change to iLOAD or pLOAD in the future
        else
          count_next  <= count + 1;      -- 
          k_counter_next <= '0';
          state_next  <= sTag;
        end if;
  
      when others =>
        state_next  <= sLoad;
        k_counter_next <= '0';
        count_next  <= count_reset;
  
    end case;
  
  end process;
  
end architecture;  
