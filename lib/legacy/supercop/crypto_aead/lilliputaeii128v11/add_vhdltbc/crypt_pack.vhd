-- Implementation of the Lilliput-TBC tweakable block cipher by the
-- Lilliput-AE team, hereby denoted as "the implementer".
--
-- For more information, feedback or questions, refer to our website:
-- https://paclido.fr/lilliput-ae
--
-- To the extent possible under law, the implementer has waived all copyright
-- and related or neighboring rights to the source code in this file.
-- http://creativecommons.org/publicdomain/zero/1.0/;

library IEEE;
library work;
use IEEE.STD_LOGIC_1164.ALL;
use work.const_pack.ALL;

package crypt_pack is

    subtype bit2 is std_logic_vector(1 downto 0);
    subtype bit4 is std_logic_vector(3 downto 0);
    subtype bit8 is std_logic_vector(7 downto 0);
    subtype bit16 is std_logic_vector(15 downto 0);
    subtype bit32 is std_logic_vector(31 downto 0);
    subtype bit64 is std_logic_vector(63 downto 0);
    subtype bit128 is std_logic_vector(127 downto 0);
    subtype bit256 is std_logic_vector(255 downto 0);
    subtype bit192 is std_logic_vector(191 downto 0);
    subtype bit80 is std_logic_vector(79 downto 0);

    subtype bit_nonce is std_logic_vector(NONCE_LEN-1 downto 0);
    subtype bit_tag is std_logic_vector(TAG_LEN-1 downto 0);
    subtype bit_data is std_logic_vector(DATA_LEN-1 downto 0);
    subtype bit_tweak is std_logic_vector(TWEAK_LEN-1 downto 0);
    subtype bit_key is std_logic_vector(KEY_LEN-1 downto 0);
    subtype bit_tweak_key is std_logic_vector((TWEAK_LEN+KEY_LEN)-1 downto 0);
    subtype bit_data_in is std_logic_vector(DATA_IN_LEN-1 downto 0);
    subtype bit_tweak_in is std_logic_vector(TWEAK_IN_LEN-1 downto 0);
    subtype bit_key_in is std_logic_vector(sw-1 downto 0);
    subtype bit_data_out is std_logic_vector(DATA_OUT_LEN-1 downto 0);

    type row_b8 is array(0 to 7) of bit8;

    type row_state is array(0 to 3) of bit8;
    type type_half_state is array(0 to 1) of row_state;
    type type_state is array(0 to 3) of row_state;

    type key_row_state is array(0 to 3) of bit8;
    type type_key is array(0 to 1) of key_row_state;
    type type_stored_key is array(0 to ROUND_NB-1) of type_key;


    type type_output_vector is array(0 to DATA_LEN/w-1) of bit_data_out;

    type type_tweak_key_row is array(0 to 7) of bit8;
    type type_tweak_key_array is array(0 to ((TWEAK_LEN+KEY_LEN)/64)-1) of type_tweak_key_row;
	 type type_key_array       is array(TWEAK_LEN/64 to ((TWEAK_LEN+KEY_LEN)/64)-1) of type_tweak_key_row;

    type keyschedule_row_state is array(0 to 3) of bit8;
    type type_keyschedule is array(0 to 3) of keyschedule_row_state;

    constant Wdiv8          : integer := W/8;
    constant ROUND          : integer := ROUND_NB;
    constant INOUT_CYCLE_NB : integer := (KEY_LEN/sw)-1;
    constant TWEAK_KEY_LEN  : integer := TWEAK_LEN+KEY_LEN-1;
    constant LANE_NB        : integer := ((TWEAK_LEN+KEY_LEN)/64);
	 constant LANE_Y_BEGIN_NB : integer := (TWEAK_LEN/64);
    constant DATA_IN_LOOP   : integer := (DATA_LEN/DATA_IN_LEN)-1;
    constant KEY_IN_LOOP    : integer := (KEY_LEN/sw)-1;
    constant TWEAK_IN_LOOP  : integer := (TWEAK_LEN/TWEAK_IN_LEN)-1;
    constant DATA_OUT_LOOP  : integer := (DATA_LEN/DATA_OUT_LEN)-1;
end crypt_pack; 