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

use std.textio.all;
library ieee;
use ieee.math_real.all;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_textio.all;

use work.util_unsynth.all;
use work.wage_pkg.all;

package wage_unsynth is

  constant max_data_sz     : integer := 320;
  -- ******************************************** --
  type tag_ty is ( KEY_TAG, NONCE_TAG, INPUT_TAG, OUTPUT_TAG, AD_TAG, PLAINTEXT_TAG, CIPHERTEXT_TAG, EOF_TAG, NULL_TAG );

  -- function to_hex_char_normal( v : std_logic_vector ) return character;
  function to_hex_string_normal( v : std_logic_vector ) return string;
  -- ******************************************** --
  function is_tag( c : character ) return boolean;
  function chr_to_tag( chr : character ) return tag_ty;
  -- ******************************************** --
  procedure check_tag
    ( spec, impl : tag_ty );

  procedure read_next_tag_data_line_normal
    ( file f   : text;
      tag      : out tag_ty;
      data     : out std_logic_vector;
      num_bits : out natural );
  procedure hread_dir_normal
    ( ln       : inout line;
      result   : out   std_logic_vector;
      num_bits : out   natural
    );

  function m_to_data_io	(m : std_logic_vector( 0 to 63 ) ) return data_io;
  function data_io_vector_to_tag ( s : data_io_vector( 0 to 8 ) ) return std_logic_vector;
  function kn_to_data_io_vector (key_stim, nonce_stim : std_logic_vector( 0 to 127 ) ) return data_io_vector;
  function flip_state ( s : state_array ) return state_array;
  function state_to_vector (s : state_array) return std_logic_vector;
  function pad ( v : std_logic_vector ) return std_logic_vector;

  type   state_array_temp is array (8 downto 0) of std_logic_vector(31 downto 0);

  function perfect (s : std_logic_vector) return state_array_temp;
  function data_io_to_m (s : data_io ) return std_logic_vector;

  procedure drive_reset
    ( signal   clk   : in std_logic
    ; signal   reset : out std_logic
    ; constant hold  : in time
    );

  procedure read_key_nonce
    ( key_stim       : out std_logic_vector
    ; nonce_stim     : out std_logic_vector
    ; stim_file_path : in string
    );

  procedure drive_data
    ( signal   clk     : in std_logic
    ; variable data    : in std_logic_vector
    ; constant hold    : in time
    ; signal   i_data  : out std_logic_vector
    ; signal   i_valid : out std_logic
    );

  procedure drive_all
    ( constant target_tag : in tag_ty
    ; stim_file_path      : in string
    ; constant hold       : in time
    ; constant delay      : in time
    ; constant manual_pad : in boolean
    ; signal clk          : in std_logic
    ; signal o_ready      : in std_logic
    ; signal o_data       : in std_logic_vector
    ; signal i_data       : out std_logic_vector
    ; signal i_valid      : out std_logic
    ; signal i_padding    : out std_logic
    ; signal bits_pad     : out natural
    );

end package;  


package body wage_unsynth is

  procedure drive_reset
    ( signal   clk   : in std_logic
    ; signal   reset : out std_logic
    ; constant hold  : in time
    )
  is
  begin

    wait until rising_edge(clk);
    wait for hold;
    reset <= '1';                                                
    wait until rising_edge(clk);
    wait for hold;
    reset <= '0';

  end procedure;


  procedure read_key_nonce
    ( key_stim          : out std_logic_vector
    ; nonce_stim        : out std_logic_vector
    ; stim_file_path    : in string
    )
  is
    variable tag       : tag_ty;
    file     stim_file : text;
    variable num_bits  : natural;
  begin

    file_close(stim_file);
    file_open(stim_file, stim_file_path, read_mode);

    tag := NULL_TAG;
    while tag /= KEY_TAG and tag /= EOF_TAG loop
      read_next_tag_data_line_normal( stim_file, tag, key_stim, num_bits );
    end loop;

    if tag = KEY_TAG then
      if num_bits /= 128 then
        file_close(stim_file);
        assert false
        report( "Key length = " & natural'image(num_bits) & " bits, must be 128 bits")
        severity failure;
      else
        report( "Key length = " & natural'image(num_bits) & " bits");
      end if;
    else
      file_close(stim_file);
      assert false
      report ("Key isn't found in file " & stim_file_path)
      severity failure;
    end if;

    file_close(stim_file);
    file_open(stim_file, stim_file_path, read_mode);

    tag := NULL_TAG;
    while tag /= NONCE_TAG and tag /= EOF_TAG loop
      read_next_tag_data_line_normal( stim_file, tag, nonce_stim, num_bits );
    end loop;

    if tag = NONCE_TAG then
      if num_bits /= 128 then
        file_close(stim_file);
        assert false
        report( "Nonce length = " & natural'image(num_bits) & " bits, must be 128 bits")
        severity failure;
      else
        report( "Nonce length = " & natural'image(num_bits) & " bits");
      end if;
    else
      file_close(stim_file);
      assert false
      report ("Nonce isn't found in file " & stim_file_path)
      severity failure;
    end if;

    file_close(stim_file);

  end procedure;


  procedure drive_data
    ( signal   clk      : in std_logic
    ; variable data     : in std_logic_vector
    ; constant hold     : in time
    ; signal   i_data   : out std_logic_vector
    ; signal   i_valid  : out std_logic
    )
  is
  begin

    i_data   <= data;
    i_valid  <= '1';
    wait until rising_edge(clk);
    wait for hold;
    i_valid  <= '0';

  end procedure;



  procedure drive_all
    ( constant target_tag : in tag_ty
    ; stim_file_path      : in string
    ; constant hold       : in time
    ; constant delay      : in time
    ; constant manual_pad : in boolean
    ; signal clk          : in std_logic
    ; signal o_ready      : in std_logic
    ; signal o_data       : in std_logic_vector
    ; signal i_data       : out std_logic_vector
    ; signal i_valid      : out std_logic
    ; signal i_padding    : out std_logic
    ; signal bits_pad     : out natural
    )
  is
    variable tag : tag_ty;
    variable num_bits, num_bits_pad : natural;
    variable data, data_buf : std_logic_vector(0 to 63);
    variable data128 : std_logic_vector(0 to 127);
    file     stim_file : text;
    variable break : boolean;
  begin

    break := false;
    i_padding <= '0';
    num_bits_pad := 64;
    bits_pad <= 64;    

    file_close(stim_file);
    file_open(stim_file, stim_file_path, read_mode);

    tag := NULL_TAG;
    while tag /= target_tag and tag /= EOF_TAG loop
      read_next_tag_data_line_normal( stim_file, tag, data128, num_bits );
    end loop;

    while tag /= EOF_TAG and not(break) loop

      report( "Data length = " & natural'image(num_bits) & " bits");

      data := data128(0 to 63);

      if data(63) /= '1' and data(63) /= '0' and manual_pad = true then
        break := true;
      end if;


      data := pad(data);
      num_bits_pad := num_bits;

      wait for delay;

      if num_bits > 64 then
        bits_pad <= 64;
      else
        bits_pad <= num_bits;       
      end if;

      if break then 
        i_data <= data;                         -- ciphertext goes here
        i_valid                <= '1';
        i_padding <= '1';                       ---- handle the padding
        wait until rising_edge(clk);
        data_buf := o_data;                      ---- handle the padding
        wait for hold;
        i_valid                <= '0';
      else
        drive_data( clk, data, hold, i_data, i_valid );
        wait until o_ready = '1' and rising_edge(clk);
      end if;

      if not(break) then

        data := data128(64 to 127);

        if num_bits > 64 then    -- or if num_bits > 64, i.e. second 64-bits exist

          if data(63) /= '1' and data(63) /= '0' and manual_pad = true then
            break := true;
            num_bits_pad := num_bits;
          end if;

          data := pad(data);
          wait for delay;

          bits_pad <= num_bits - 64;    

          if break then 
            i_data <= data;                         -- ciphertext goes here
            i_valid                <= '1';
            i_padding <= '1';                       ---- handle the padding
            wait until rising_edge(clk);
            data_buf := o_data;                      ---- handle the padding
            wait for hold;
            i_valid                <= '0';
          else
            drive_data( clk, data, hold, i_data, i_valid );
            wait until o_ready = '1' and rising_edge(clk);
          end if;

        end if;

      end if;

      tag := NULL_TAG;
      while tag /= target_tag and tag /= EOF_TAG loop
        read_next_tag_data_line_normal( stim_file, tag, data128, num_bits );
      end loop;

    end loop;


    if break then
      if num_bits_pad > 64 then num_bits_pad := num_bits_pad - 64; end if;

      report ("last ciphertext block is " & natural'image(num_bits_pad) & " bits + padding is " & natural'image(64 - num_bits_pad) & " bits");
      report ("I_DATA (ciphertext) is: " & to_hex_string_normal( data ) );
      report ("O_DATA BUF (plaintext) is: " & to_hex_string_normal( data_buf ) );

      data := data(0 to num_bits_pad-1) & data_buf(num_bits_pad to 63);         
      report ("PADDED I_DATA (for replace) is: " & to_hex_string_normal( data ) );
      wait for 20*hold;
      wait until rising_edge(clk);
      wait for hold;

      i_data     <= data;         --- ciphertext with "replaced" tag goes here
      i_valid    <= '1';
      i_padding  <= '0';
      wait until rising_edge(clk);
      wait for hold;
      i_valid        <= '0';

      wait until o_ready = '1' and rising_edge(clk);
    end if;

    break := false;
    file_close(stim_file);

  end procedure;











  function data_io_to_m (s : data_io ) return std_logic_vector is
    variable z : std_logic_vector (0 to 69);
    variable z2 : std_logic_vector (0 to 63);
  begin

    for i in 0 to 9 loop
      for j in 0 to 6 loop
         z(i*7+j) := s(i)(j);
      end loop;
    end loop;

  z2 := z(0 to 63);
  return z2;

  end function;

  -- ******************************************** --
  -- ******************************************** --
  function m_to_data_io	(m : std_logic_vector( 0 to 63 ) ) return data_io is
    variable z	: data_io;
  begin

    for i in 0 to 8 loop
      z(i) := m(i*7 to i*7+6);
    end loop;

    z(9) := m(63) & "000000";
    return z;

  end function;
  -- ******************************************** --
  -- ******************************************** --
  function data_io_vector_to_tag ( s : data_io_vector(0 to 8) ) return std_logic_vector is
    variable z	: std_logic_vector( 0 to 127 );
  begin
    
    for i in 0 to 8 loop
      z(2*i*7 to 2*i*7 + 6) := s(i)(6);		-- T0 to T16
    end loop;

    for i in 0 to 7 loop
      z((2*i+1)*7 to (2*i+1)*7 + 6) := s(i)(1);	-- T1 to T15
    end loop;

    z(17*7 to 17*7 + 6) := s(1)(3);		-- T17
    z(126 to 127)	:= s(2)(3)(2 to 3);	-- T18

    return z;

  end function;
  -- ******************************************** --
  -- ******************************************** --
  function kn_to_data_io_vector (key_stim, nonce_stim : std_logic_vector( 0 to 127 ) )
  return data_io_vector is
    variable z		: data_io_vector(0 to 8);
    variable n_vector	: gf_vector(0 to 17);
    variable k_vector	: gf_vector(0 to 17);
    variable m_vector	: gf_vector(0 to 17);
    variable k18	: gf_elem;
  begin

    n_vector := (others => (others => '0') );
    k_vector := (others => (others => '0') );

    for i in 0 to 8 loop
      for j in 0 to gf_dim-1 loop
	n_vector(i)(j) := nonce_stim(7*i+j);
	k_vector(i)(j) := key_stim(7*i+j);
      end loop;
    end loop;

    for i in 9 to 17 loop
      for j in 0 to gf_dim-1 loop
	n_vector(i)(j) := nonce_stim(7*i+1+j);
	k_vector(i)(j) := key_stim(7*i+1+j);
      end loop;
    end loop;

    k18 := key_stim(63) & key_stim(127) & nonce_stim(63) & nonce_stim(127) & "000";
    z := (others => (others => (others => '0')));

    for i in 0 to 8 loop
      z(i)(0) := k_vector(2*i);
      z(i)(5) := k_vector(1+2*i);
      z(i)(8) := n_vector(2*i);
    end loop;

    for i in 1 to 8 loop
      z(i)(3) := n_vector(2*i-1);
    end loop;

    z(7)(4) := n_vector(15);
    z(8)(4) := k18;

    z(7)(3) := n_vector(13);
    z(8)(3) := n_vector(17);

    return z;

  end function;
  -- ******************************************** --
  -- ******************************************** --
  function flip_state ( s : state_array ) return state_array is
    variable z : state_array;
  begin

    for i in 0 to N-1 loop
      for j in 0 to gf_dim-1 loop
        z(i)(j) := s(i)(gf_dim-1-j);
      end loop;
    end loop;

    return z;
 
  end function;
  -- ******************************************** --
  -- ******************************************** --
  function state_to_vector (s : state_array) return std_logic_vector is
    variable z : std_logic_vector(N*gf_dim-1 downto 0);
  begin
    
    for i in 0 to N-1 loop
      for j in 0 to gf_dim-1 loop
        z(gf_dim*i+j) := s(i)(j);
      end loop;
    end loop;

  return z;

  end function;
  -- ******************************************** --
  function perfect (s : std_logic_vector) return state_array_temp is
    variable z : state_array_temp;
  begin
    
    z := (others => (others => 'X'));
    z(8) := (others => '0');

    for i in 8 downto 0 loop
      for j in 31 downto 0 loop
        if 32*i+j >= 28 and 32*i+j <= 286 then z(i)(j) := s(32*i+j-28); end if;
      end loop;
    end loop;

  z(0)(27 downto 0) := (others => '0');

  return z;

  end function;
  -- ******************************************** --
  function pad ( v : std_logic_vector ) return std_logic_vector is
    variable z : std_logic_vector(v'low to v'high);
  begin
    for i in v'high downto v'low + 1 loop
      if v(i) /= '1' and v(i) /= '0' then
	if v(i-1) = '1' or v(i-1) = '0' then
	  z(i) := '1';
	else
	  z(i) := '0';
	end if;
      else
	z(i) := v(i);
      end if;
    end loop;
    z(0) := v(0);
    return(z);
  end function;
  -- ******************************************** --
  -- ******************************************** --
  function to_hex_string_normal( v : std_logic_vector ) return string is
  begin
    if v'length <= 4 then
      return to_hex_char( rev( v ) ) & "";				--<----- added rev( change 1/3)
    else
      if v'ascending then
        -- report( "to_hex_string: ascending" );
        return to_hex_char( rev( v( v'low to v'low+3)) )		--<----- added rev( change 2/3)
               & to_hex_string_normal( v(v'low+4 to v'high) );
      else
        -- report( "to_hex_string: descending" );
        return to_hex_char( rev( v( v'high downto v'high-3)) )		--<----- added rev( change 3/3)
               & to_hex_string_normal( v(v'high-4 downto v'low) );
      end if;
    end if;
  end function;
  -- ******************************************** --
  -- ******************************************** --
  function is_tag( c : character ) return boolean is
  begin
    return c = 'K' or c = 'N' or c = 'I' or c = 'O' or c = 'A' or c = 'P' or c = 'C';
  end function;
  -- ******************************************** --
  -- ******************************************** --
  function chr_to_tag( chr : character ) return tag_ty is
  begin
    case chr is
      when 'K' => return KEY_TAG;
      when 'N' => return NONCE_TAG;
      when 'I' => return INPUT_TAG;
      when 'O' => return OUTPUT_TAG;
      when 'A' => return AD_TAG;
      when 'P' => return PLAINTEXT_TAG;
      when 'C' => return CIPHERTEXT_TAG;
      when others =>
        assert false
          report ("chr_to_tag: not a tag: "& character'image( chr ))
          severity error;
        return EOF_TAG;
    end case;
  end function;
  -- ******************************************** --
  -- ******************************************** --
  procedure check_tag
   ( spec, impl : tag_ty )
  is
 
  begin
    if spec /= impl then
      assert false
      report( "check_tag: expected tag="& tag_ty'image(spec)
              &", but found tag="& tag_ty'image(impl) )
      severity warning; --error
    else
      report( "else from check_tag" );
    end if;
  end procedure;
  -- ******************************************** --
  -- ******************************************** --
  procedure read_next_tag_data_line_normal
    ( file f   : text;
      tag      : out tag_ty;
      data     : out std_logic_vector;
      num_bits : out natural )
  is
    variable found
           , ok            : boolean;
    variable input_line    : line;
    variable chr           : character;
    variable tmp_tag       : tag_ty;
    variable tmp_data      : std_logic_vector( data'range );
    variable tmp_num_bits  : natural;
  begin
    -- read lines until hit one where first non-blank character is a tag
    found := false;
    while not found and not endfile( f ) loop
      readline( f, input_line ); 
      read( input_line, chr, good => ok );
      -- skip blanks and comments
      found := ok and is_tag(chr);
    end loop;
    if not found then
      tmp_tag  := EOF_TAG;
      tmp_data := (others => 'U');
    else
      tmp_tag  := chr_to_tag( chr );
      hread_dir_normal( input_line, tmp_data, tmp_num_bits );     -- <––– added _normal (change 1/1)
      -- hread( input_line, tmp_data );
    end if;
    if tmp_num_bits > 0 then 
      report( "read_next_tag_data: --> "& tag_ty'image(tmp_tag) &" "&
              to_string( firstn( tmp_data, tmp_num_bits ) ) );
    else
      report( "read_next_tag_data: --> "& tag_ty'image(tmp_tag) &" 0 bits");
    end if;
    tag      := tmp_tag;
    data     := tmp_data;
    num_bits := tmp_num_bits;
  end procedure;
  -- ******************************************** --
  -- ******************************************** --
 procedure hread_dir_normal
    ( ln       : inout line;
      result   : out   std_logic_vector;
      num_bits : out   natural
    )
  is
    variable ok           : boolean;
    variable chr          : character;
    variable str          : string(1 to result'length/4);
    variable tmp_result   : std_logic_vector(0 to result'length-1);
    variable num_chars,
             tmp_num_bits : natural := 0;
  begin
    
    if result'length mod 4 /= 0 then
      assert false
        report 
          "hread_dir error: trying to read vector " &
             "with non multiple of 4 length";
      return;
    end if;

    loop                                    -- skip white space
      read(ln, chr, ok);
      exit when not ok or ((chr /= ' ') and (chr /= CR) and (chr /= HT));
    end loop;
    if not ok then
      assert false
        report ("hread_dir error: failed" )
        severity error;
      return;
    else
     -- report ("hread_dir: char0="& chr );
    end if;

    num_chars        := 1;
    str( num_chars ) := chr;
    
    while ok loop
      read(ln, chr, ok);
      if ok then 
        num_chars        := num_chars + 1;
        str( num_chars ) := chr;
      end if;
    end loop;

    tmp_num_bits := num_chars * 4;
    tmp_result   := (others => 'U');
    if result'ascending then
      tmp_result( 0 to tmp_num_bits - 1) :=
        hex_string_to_dn_vector( str( 1 to num_chars ) );   -- <––– changed from up to dn (change 1/2)
                                                            -- HEX(0) should be treated MSB, HEX(3) should be treated as LSB
                                                            -- for correct interpretation of HEX
    else 
      tmp_result( 0 to tmp_num_bits - 1) :=
        hex_string_to_up_vector( str( 1 to num_chars ) );   -- <––– changed from up to dn (change 2/2)
                                                            -- HEX(0) should be treated MSB, HEX(3) should be treated as LSB
                                                            -- for correct interpretation of HEX
    end if;
    
    -- report( "hread_dir:  " & str &" -->"&
    --        to_string( tmp_result( 0 to tmp_num_bits - 1 ) ) );
    
    result   := tmp_result;
    num_bits := tmp_num_bits;
  end procedure; 
  -- ******************************************** --
  -- ******************************************** --
  -- ******************************************** --
  -- ******************************************** --

  -- ******************************************** --
  -- ******************************************** --

  -- ******************************************** --
  -- ******************************************** --

  -- ******************************************** --
  -- ******************************************** --

end package body;
