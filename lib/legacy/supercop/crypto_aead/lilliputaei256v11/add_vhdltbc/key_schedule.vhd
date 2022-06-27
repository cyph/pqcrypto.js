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
use IEEE.numeric_std.ALL;
use IEEE.STD_LOGIC_1164.ALL;
use work.crypt_pack.ALL;

entity key_schedule_liliput is
	port (
		key_i        : in  type_tweak_key_array;
		round_number : in  std_logic_vector(7 downto 0);
		invert_i     : in  std_logic;
		key_o        : out type_tweak_key_array;
		round_key_o  : out type_key
	);
end key_schedule_liliput;

architecture key_schedule_liliput_arch of key_schedule_liliput is

	component multiplications
		port(
			mularray_i : in  type_tweak_key_array;
			mularray_o : out type_tweak_key_array
		);
	end component;

	component inv_multiplication
		port(
			mularray_i : in  type_tweak_key_array;
			mularray_o : out type_tweak_key_array
		);
	end component;

	signal key_s       : type_tweak_key_array;
	signal key_s_inv   : type_tweak_key_array;
	signal round_key_s : type_key;

begin

	multiplications_t : multiplications
		port map (
			mularray_i => key_i,
			mularray_o => key_s
		);

	inv_multiplications_t : inv_multiplication
		port map (
			mularray_i => key_i,
			mularray_o => key_s_inv
		);

	key_o <= key_s when invert_i = '0' else
		key_s_inv;

	if_lane4 : if LANE_NB=4 generate
		col2 : for j in 0 to 3 generate
			round_key_s(0)(j) <= key_i(0)(j) xor key_i(1)(j) xor key_i(2)(j) xor key_i(3)(j) ;
			round_key_s(1)(j) <= key_i(0)(j+4) xor key_i(1)(j+4) xor key_i(2)(j+4) xor key_i(3)(j+4);
		end generate;
	end generate;

	if_lane5 : if LANE_NB=5 generate
		col2 : for j in 0 to 3 generate
			round_key_s(0)(j) <= key_i(0)(j) xor key_i(1)(j) xor key_i(2)(j) xor key_i(3)(j) xor key_i(4)(j) ;
			round_key_s(1)(j) <= key_i(0)(j+4) xor key_i(1)(j+4) xor key_i(2)(j+4) xor key_i(3)(j+4) xor key_i(4)(j+4);
		end generate;
	end generate;

	if_lane6 : if LANE_NB=6 generate
		col2 : for j in 0 to 3 generate
			round_key_s(0)(j) <= key_i(0)(j) xor key_i(1)(j) xor key_i(2)(j) xor key_i(3)(j) xor key_i(4)(j) xor key_i(5)(j) ;
			round_key_s(1)(j) <= key_i(0)(j+4) xor key_i(1)(j+4) xor key_i(2)(j+4) xor key_i(3)(j+4) xor key_i(4)(j+4) xor key_i(5)(j+4);
		end generate;
	end generate;

	if_lane7 : if LANE_NB=7 generate
		col2 : for j in 0 to 3 generate
			round_key_s(0)(j) <= key_i(0)(j) xor key_i(1)(j) xor key_i(2)(j) xor key_i(3)(j) xor key_i(4)(j) xor key_i(5)(j) xor key_i(6)(j) ;
			round_key_s(1)(j) <= key_i(0)(j+4) xor key_i(1)(j+4) xor key_i(2)(j+4) xor key_i(3)(j+4) xor key_i(4)(j+4) xor key_i(5)(j+4) xor key_i(6)(j+4);
		end generate;
	end generate;


	round_key_o(0)(0) <= round_key_s(0)(0) xor round_number;
	round_key_o(0)(1) <= round_key_s(0)(1);
	round_key_o(0)(2) <= round_key_s(0)(2);
	round_key_o(0)(3) <= round_key_s(0)(3);
	round_key_o(1)    <= round_key_s(1);


end key_schedule_liliput_arch;


configuration key_schedule_liliput_conf of key_schedule_liliput is
	for key_schedule_liliput_arch
		for multiplications_t : multiplications
			use entity work.multiplications(Behavioral);
		end for;
		for inv_multiplications_t : inv_multiplication
			use entity work.inv_multiplication(inv_multiplication_arch);
		end for;
	end for;
end configuration key_schedule_liliput_conf ;
