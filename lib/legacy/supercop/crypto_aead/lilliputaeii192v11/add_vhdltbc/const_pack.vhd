-- Implementation of the Lilliput-TBC tweakable block cipher by the
-- Lilliput-AE team, hereby denoted as "the implementer".
--
-- For more information, feedback or questions, refer to our website:
-- https://paclido.fr/lilliput-ae
--
-- To the extent possible under law, the implementer has waived all copyright
-- and related or neighboring rights to the source code in this file.
-- http://creativecommons.org/publicdomain/zero/1.0/library IEEE;

library IEEE;
library work;
use IEEE.STD_LOGIC_1164.ALL;

package const_pack is
	--Lilliput constants
	constant NONCE_LEN : integer := 120;
	constant TAG_LEN   : integer := 128;
	constant DATA_LEN  : integer := 128;

	--Lilliput parameters
	constant ROUND_NB  : integer := 36;
	constant TWEAK_LEN : integer := 128;
	constant KEY_LEN   : integer := 192;

	-- lenght of inputs
	constant DATA_IN_LEN  : integer := 32;
	constant KEY_IN_LEN   : integer := 32;
	constant TWEAK_IN_LEN : integer := 32;
	constant DATA_OUT_LEN : integer := 32;
	constant sw           : integer := 32;
	constant W            : integer := 32;


	-- Segment Type Encoding
	constant TYPE_AD    : std_logic_vector(3 downto 0) := "0001";
	constant TYPE_MES   : std_logic_vector(3 downto 0) := "0100";
	constant TYPE_CT    : std_logic_vector(3 downto 0) := "1001";
	constant TYPE_TAG   : std_logic_vector(3 downto 0) := "1000";
	constant TYPE_NONCE : std_logic_vector(3 downto 0) := "1100";
end const_pack;
