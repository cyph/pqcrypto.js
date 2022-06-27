-- Implementation of the Lilliput-TBC tweakable block cipher by the
-- Lilliput-AE team, hereby denoted as "the implementer".
--
-- For more information, feedback or questions, refer to our website:
-- https://paclido.fr/lilliput-ae
--
-- To the extent possible under law, the implementer has waived all copyright
-- and related or neighboring rights to the source code in this file.
-- http://creativecommons.org/publicdomain/zero/1.0/

library IEEE;
library work;
use IEEE.numeric_std.all;
use IEEE.std_logic_1164.all;
use work.crypt_pack.all;


entity top_tb is
end top_tb;

architecture top_tb_arch of top_tb is

  function to_hstring (value : STD_LOGIC_VECTOR) return STRING is
    constant ne     : INTEGER := (value'length+3)/4;
    variable pad    : STD_LOGIC_VECTOR(0 to (ne*4 - value'length) - 1);
    variable ivalue : STD_LOGIC_VECTOR(0 to ne*4 - 1);
    variable result : STRING(1 to ne);
    variable quad   : STD_LOGIC_VECTOR(0 to 3);
  begin
    if value (value'left) = 'Z' then
      pad := (others => 'Z');
    else
      pad := (others => '0');
    end if;
    ivalue := pad & value;
    for i in 0 to ne-1 loop
      quad := To_X01Z(ivalue(4*i to 4*i+3));
      case quad is
        when x"0"   => result(i+1)   := '0';
        when x"1"   => result(i+1)   := '1';
        when x"2"   => result(i+1)   := '2';
        when x"3"   => result(i+1)   := '3';
        when x"4"   => result(i+1)   := '4';
        when x"5"   => result(i+1)   := '5';
        when x"6"   => result(i+1)   := '6';
        when x"7"   => result(i+1)   := '7';
        when x"8"   => result(i+1)   := '8';
        when x"9"   => result(i+1)   := '9';
        when x"A"   => result(i+1)   := 'A';
        when x"B"   => result(i+1)   := 'B';
        when x"C"   => result(i+1)   := 'C';
        when x"D"   => result(i+1)   := 'D';
        when x"E"   => result(i+1)   := 'E';
        when x"F"   => result(i+1)   := 'F';
        when "ZZZZ" => result(i+1) := 'Z';
        when others => result(i+1) := 'X';
      end case;
    end loop;
    return result;
  end function to_hstring;

  component top is port (
      start_i        : in  std_logic;
      clock_i        : in  std_logic;
      reset_i        : in  std_logic;
      data_i         : in  bit_data;
      key_i          : in  bit_key;
      data_o         : out bit_data;
      tweak_i        : in  bit_tweak;
      decrypt_i      : in  std_logic;
      liliput_on_out : out std_logic;
      valid_o        : out std_logic
    );
  end component;
  type array_data is array(0 to 1) of bit_data;
  type array_tweak is array(0 to 1) of bit_tweak;
  type array_key is array(0 to 1) of bit_key;
  type array_decrypt is array(0 to 1) of std_logic;

  signal data_vect    : array_data;
  signal key_vect     : array_key;
  signal tweak_vect   : array_tweak;
  signal decrypt_vect : array_decrypt;
  signal res_vect     : array_data;


  signal start_i_s, clock_i_s, reset_i_s : std_logic := '0';
  signal data_i_s                        : bit_data;
  signal key_i_s                         : bit_key;
  signal tweak_i_s                       : bit_tweak;
  signal data_o_s                        : bit_data;
  signal liliput_on_o_s                  : std_logic;
  signal decrypt_s                       : std_logic;
  signal valid_s                         : std_logic;

begin
  DUT : top
    port map(
      start_i        => start_i_s,
      clock_i        => clock_i_s,
      reset_i        => reset_i_s,
      data_i         => data_i_s,
      key_i          => key_i_s,
      tweak_i        => tweak_i_s,
      data_o         => data_o_s,
      decrypt_i      => decrypt_s,
      liliput_on_out => liliput_on_o_s,
      valid_o        => valid_s
    );

  clock_i_s <= not(clock_i_s) after 100 ns;
  reset_i_s <= '0' , '1' after 50 ns;

  simulation : process

    procedure check (data : in bit_data;
        key         : in bit_key;
        tweak       : in bit_tweak;
        decrypt     : in std_logic;
        res_expeted : in bit_data) is

      variable res : bit_data;

    begin
      data_i_s  <= data;
      key_i_s   <= key;
      tweak_i_s <= tweak;
      decrypt_s <= decrypt;
      start_i_s <= '1';

      wait until valid_s = '1';

      res := data_o_s;
      assert res = res_expeted
        report "Unexpected result: " &
        "Data = " & to_hstring(data) & "; " &
        "key = " & to_hstring(key) & "; " &
        "tweak = " & to_hstring(tweak) & "; " &
        "decrypt = " & std_logic'image(decrypt) & "; " &
        "res_expeted = " & to_hstring(res_expeted)& "; "
        severity error;

      data_i_s  <= (others => '0');
      key_i_s   <= (others => '0');
      tweak_i_s <= (others => '0');
      decrypt_s <= '0';
      start_i_s <= '0';

      wait for 30 ns;

    end procedure check;

  begin
    data_vect    <= (X"F0E0D0C0B0A090807060504030201000",X"F0E0D0C0B0A090807060504030201000");
    key_vect     <= (X"F1E1D1C1B1A191817161514131211101F0E0D0C0B0A090807060504030201000",X"F1E1D1C1B1A191817161514131211101F0E0D0C0B0A090807060504030201000");
    tweak_vect   <= (X"F0E0D0C0B0A090807060504030201000",X"F0E0D0C0B0A090807060504030201000");
    decrypt_vect <= ('0','1');
    res_vect     <= (X"E717B950F69C0E4E1CBDF9F2402CDBA0",X"E717B950F69C0E4E1CBDF9F2402CDBA0");

    wait for 30 ns;

    check(data_vect(0),key_vect(0),tweak_vect(0),decrypt_vect(0),res_vect(0));
    check(data_vect(1),key_vect(1),tweak_vect(1),decrypt_vect(1),res_vect(1));
    wait;
  end process simulation;
end architecture top_tb_arch;

configuration top_tb_conf of top_tb is
  for top_tb_arch
    for DUT : top
      use entity work.top(top_arch);
    end for;
  end for;
end configuration top_tb_conf;
