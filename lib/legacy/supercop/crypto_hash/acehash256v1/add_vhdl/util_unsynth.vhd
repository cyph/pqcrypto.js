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

------------------------------------------------------------------------
-- the string <-> std_logic_vector conversion functions here differ
-- from the IEEE standard.  The IEEE standard interprets the
-- leftmost bit as the MSB, independent of the direction (to/downto)
-- of the vector.  The functions here treat the 'high bit as the MSB
-- and the 'low bit as the LSB.
------------------------------------------------------------------------


package util_unsynth is

  function rev ( v : std_logic_vector ) return std_logic_vector;
  function firstn ( v : std_logic_vector; w : natural ) return std_logic_vector;

  function to_hex_char( v : std_logic_vector ) return character;
  function hex_char_to_up_vector( chr : character ) return std_logic_vector;
  function hex_char_to_dn_vector( chr : character ) return std_logic_vector;
  function hex_string_to_up_vector( str : string ) return std_logic_vector;
  function hex_string_to_dn_vector( str : string ) return std_logic_vector;
  --------------------------------------------------------------
  procedure hread_dir
    ( ln       : inout line;
      result   : out   std_logic_vector;
      num_bits : out   natural
    );
  --------------------------------------------------------------
  function to_hex_string( v : std_logic_vector ) return string;

end package;

package body util_unsynth is

  function rev( v : std_logic_vector ) return std_logic_vector is
    variable q_up : std_logic_vector( v'low to v'high );
    variable q_dn : std_logic_vector( v'high downto v'low );
  begin
    if v'ascending then
      for i in v'range loop
        q_up(i) := v( v'high - i + v'low );
      end loop;
      return q_up;
    else
      for i in v'range loop
        q_dn(i) := v( v'high - i + v'low );
      end loop;
      return q_dn;
    end if;
  end function;
  
  --------------------------------------------------------------
  
  function firstn ( v : std_logic_vector; w : natural )
    return std_logic_vector
  is
  begin
    if v'ascending then
      return v( v'low to v'low + w - 1 );
    else
      return v( v'high downto v'high - w + 1 );
    end if;
  end function;

  --------------------------------------------------------------
  
  function is01( b : std_logic ) return boolean is
  begin
    return b = '0' or b = '1';
  end function;
  
  --------------------------------------------------------------
  
  function is01( b : character ) return boolean is
  begin
    return b = '0' or b = '1';
  end function;
  
  --------------------------------------------------------------
  
  function is01( v : std_logic_vector ) return boolean is
  begin
    if v'length > 1 then
      if v'ascending then
        return is01( v( v'low ) ) and is01( v( v'low+1 to v'high) );
      else 
        return is01( v( v'high ) ) and is01( v( v'high-1 downto v'low) );
      end if;
    else
      return is01( v( v'low ) );
    end if;
  end function;

  --------------------------------------------------------------
  
  function to_hex_char( v : std_logic_vector ) return character
  is
    variable i : natural;
    variable res : character;
  begin
    assert v'length <= 4
      report ( "to_hex_char: length must be <= 4 "&
                       integer'image( v'length) )
      severity error;
    if is01( v ) then
      if v'ascending then 
        i := to_integer( unsigned( rev(v) ) );
      else
        i := to_integer( unsigned( v ) );
      end if;
      if i < 10 then
        res := character'val( character'pos('0') + i );
      else
        res := character'val( character'pos('A') + i - 10 );
      end if;
    else
      if v = "XXXX" then
        res := 'X';
      elsif v = "UUUU" then
        res := 'U';
      else
        res := '?';
      end if;
    end if;
    if v'ascending then
      null;
      -- report( "to_hex_char: ascending  : "& to_string(v) &" --> "& res );
    else
      null;
      -- report( "to_hex_char: descending : "& to_string(v) &" --> "& res );
    end if;
    return res;
  end function;

  --------------------------------------------------------------

  function to_hex_string( v : std_logic_vector ) return string is
  begin
    if v'length <= 4 then
      return to_hex_char( v ) & "";
    else
      if v'ascending then
        -- report( "to_hex_string: ascending" );
        return to_hex_char( v( v'low to v'low+3) )
               & to_hex_string( v(v'low+4 to v'high) );
      else
        -- report( "to_hex_string: descending" );
        return to_hex_char( v( v'high downto v'high-3) )
               & to_hex_string( v(v'high-4 downto v'low) );
      end if;
    end if;
  end function;
  
  --------------------------------------------------------------
  
  function aux_hex_char_to_up_vector( chr : character ) return std_logic_vector
  is
    variable result : std_logic_vector( 0 to 3 );
  begin

    case chr is
      when '0' => result := "0000";
      when '1' => result := "1000";
      when '2' => result := "0100";
      when '3' => result := "1100";
      when '4' => result := "0010";
      when '5' => result := "1010";
      when '6' => result := "0110";
      when '7' => result := "1110";
      when '8' => result := "0001";
      when '9' => result := "1001";
      when 'A' => result := "0101";
      when 'B' => result := "1101";
      when 'C' => result := "0011";
      when 'D' => result := "1011";
      when 'E' => result := "0111";
      when 'F' => result := "1111";
 
      when 'a' => result := "0101";
      when 'b' => result := "1101";
      when 'c' => result := "0011";
      when 'd' => result := "1011";
      when 'e' => result := "0111";
      when 'f' => result := "1111";
      when others => result := "XXXX";
    end case;
    
    return result;
    
  end function;
  
  --------------------------------------------------------------
  
  function hex_char_to_up_vector( chr : character ) return std_logic_vector
  is
    variable result : std_logic_vector( 0 to 3 );
  begin

    result := aux_hex_char_to_up_vector( chr );

    assert is01( result )
      report
        "hex_char_to_up_vector: error: read '" & chr &
           "', expected a hex character (0-F)."
      severity error;
    
    -- report( "hex_char_to_up_vector: " & chr &" -->"& to_string( result ) );
    
    return result;

  end function;
  
  --------------------------------------------------------------
  
  function hex_char_to_dn_vector( chr : character ) return std_logic_vector
  is
    variable result : std_logic_vector( 3 downto 0 );
  begin

    result := rev(aux_hex_char_to_up_vector( chr ));

    assert is01( result )
      report
        "hex_char_to_dn_vector: error: read '" & chr &
           "', expected a hex character (0-F)."
      severity error;
    
    -- report( "hex_char_to_dn_vector: " & chr &" -->"& to_string( result ) );
    
    return result;

  end function;
  
  --------------------------------------------------------------
  
  function hex_string_to_up_vector( str : string ) return std_logic_vector
  is
    variable result  : std_logic_vector( 0 to str'length*4 - 1);
    variable j       : integer;
  begin
    if str'ascending then 
      for i in str'low to str'high loop
        j := i - str'low;
        result( j*4 to (j+1)*4-1 ) := hex_char_to_up_vector( str(i) );
      end loop;
    else
      for i in str'left downto str'right loop
        j := str'high - i;
        result( j*4 to (j+1)*4-1 ) := hex_char_to_up_vector( str(i) );
      end loop;
    end if;
    -- report( "hex_string_to_up_vector: "& str &" --> "& to_string( result ) );
    return result;
  end function;
    
  --------------------------------------------------------------
  
  function hex_string_to_dn_vector( str : string ) return std_logic_vector
  is
    variable result  : std_logic_vector( str'length*4 - 1 downto 0);
    variable j       : integer;
  begin
    if str'ascending then
      for i in str'low to str'high loop
        j := str'high - i;
        result( (j+1)*4-1 downto j*4 ) := hex_char_to_dn_vector( str(i) );
      end loop;
    else
      for i in str'left downto str'right loop
        j := i - str'low;
        result( (j+1)*4-1 downto j*4 ) := hex_char_to_dn_vector( str(i) );
      end loop;
    end if;
    -- report( "hex_string_to_dn_vector: "& str &" --> "& to_string( result ) );
    return result;
  end function;
    
  --------------------------------------------------------------
  -- Modified versions of procedures from std_logic_texio.
  -- Modification is that these procedures order the bits
  -- independent of the direction of the vector.
  -- Example:
  -- "7" --> v : std_logic( 0 upto ... )  = "1110...."
  -- "7" --> v : std_logic( ... downto 0) = "1110...."
  
  procedure char_to_vector
    ( c      : in  character; 
      result : out std_logic_vector;
      good   : out boolean
    )
  is
    variable dn_tmp : std_logic_vector(3 downto 0);
    variable up_tmp : std_logic_vector(0 to     3);
  begin

    case c is
      when '0' => dn_tmp :=  x"0"; good := true;
      when '1' => dn_tmp :=  x"1"; good := true;
      when '2' => dn_tmp :=  x"2"; good := true;
      when '3' => dn_tmp :=  x"3"; good := true;
      when '4' => dn_tmp :=  x"4"; good := true;
      when '5' => dn_tmp :=  x"5"; good := true;
      when '6' => dn_tmp :=  x"6"; good := true;
      when '7' => dn_tmp :=  x"7"; good := true;
      when '8' => dn_tmp :=  x"8"; good := true;
      when '9' => dn_tmp :=  x"9"; good := true;
      when 'A' => dn_tmp :=  x"A"; good := true;
      when 'B' => dn_tmp :=  x"B"; good := true;
      when 'C' => dn_tmp :=  x"C"; good := true;
      when 'D' => dn_tmp :=  x"D"; good := true;
      when 'E' => dn_tmp :=  x"E"; good := true;
      when 'F' => dn_tmp :=  x"F"; good := true;
 
      when 'a' => dn_tmp :=  x"A"; good := true;
      when 'b' => dn_tmp :=  x"B"; good := true;
      when 'c' => dn_tmp :=  x"C"; good := true;
      when 'd' => dn_tmp :=  x"D"; good := true;
      when 'e' => dn_tmp :=  x"E"; good := true;
      when 'f' => dn_tmp :=  x"F"; good := true;
      when others =>
        assert false
          report
            "char_to_vector: error: read '" & c &
               "', expected a hex character (0-F).";
        good := false;
    end case;

    if result'ascending then
      up_tmp := rev( dn_tmp );
      -- report( "char_to_vector: " & c &" -->"& to_string( up_tmp ) );
      result := up_tmp;
    else
      -- report( "char_to_vector: " & c &" -->"& to_string( dn_tmp ) );
      result := dn_tmp;
    end if;
    
  end procedure;

  --------------------------------------------------------------
  
  procedure hex_string_to_vector
    ( str    : in string;
      result : out std_logic_vector
    )
  is
    constant str_len : natural := str'length;
    constant vec_len : natural := result'length/4;
  begin

    assert str_len <= vec_len
      report ("hex_string_to_vector: string longer than vector/4: "&
               integer'image( str_len )
              &" > "& integer'image(vec_len ) )
      severity warning;
    assert vec_len >= str_len
      report ("hex_string_to_vector: vector/4 longer than string : "&
                integer'image(vec_len)
              &" > "&integer'image( str_len )
              )
      severity warning;
    if result'ascending then
      result := hex_string_to_up_vector( str );
    else
      result := hex_string_to_dn_vector( str );
    end if;
  end procedure;
  
  --------------------------------------------------------------
  
  procedure hread_dir
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
        hex_string_to_up_vector( str( 1 to num_chars ) );
    else 
      tmp_result( 0 to tmp_num_bits - 1) :=
        hex_string_to_dn_vector( str( 1 to num_chars ) );
    end if;
    
    -- report( "hread_dir:  " & str &" -->"&
    --        to_string( tmp_result( 0 to tmp_num_bits - 1 ) ) );
    
    result   := tmp_result;
    num_bits := tmp_num_bits;
    
  end procedure; 

  --------------------------------------------------------------
   
end package body;  
