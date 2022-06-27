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


entity chiffrement is port (
		chiffrement_i : in  type_state;
		permutation_i : in  std_logic;
		round_key_i   : in  type_key;
		chiffrement_o : out type_state
	);

end chiffrement;

architecture chiffrement_arch of chiffrement is

	signal non_linear_s  : type_half_state;
	signal non_linear_s1 : type_half_state;
	signal linear_s      : type_half_state;
	signal linear_tmp_s  : type_half_state;
	signal linear        : bit8;

	component sbox
		port (
			sbox_i : in  bit8;
			sbox_o : out bit8
		);
	end component;


begin

	non_linear_s(0)(0) <= chiffrement_i(1)(3) xor round_key_i(1)(3);
	non_linear_s(0)(1) <= chiffrement_i(1)(2) xor round_key_i(1)(2);
	non_linear_s(0)(2) <= chiffrement_i(1)(1) xor round_key_i(1)(1);
	non_linear_s(0)(3) <= chiffrement_i(1)(0) xor round_key_i(1)(0);
	non_linear_s(1)(0) <= chiffrement_i(0)(3) xor round_key_i(0)(3);
	non_linear_s(1)(1) <= chiffrement_i(0)(2) xor round_key_i(0)(2);
	non_linear_s(1)(2) <= chiffrement_i(0)(1) xor round_key_i(0)(1);
	non_linear_s(1)(3) <= chiffrement_i(0)(0) xor round_key_i(0)(0);


	boucle_ligne : for i in 0 to 1 generate
		boucle_colonne : for j in 0 to 3 generate
				sboxx : sbox port map(
					sbox_i => non_linear_s(i)(j),
					sbox_o => non_linear_s1(i)(j)
				);
		end generate;
	end generate;

	linear_tmp_s(0)(0) <= chiffrement_i(2)(0);
	linear_tmp_s(0)(1) <= chiffrement_i(2)(1) xor chiffrement_i(1)(3);
	linear_tmp_s(0)(2) <= chiffrement_i(2)(2) xor chiffrement_i(1)(3);
	linear_tmp_s(0)(3) <= chiffrement_i(2)(3) xor chiffrement_i(1)(3);
	linear_tmp_s(1)(0) <= chiffrement_i(3)(0) xor chiffrement_i(1)(3);
	linear_tmp_s(1)(1) <= chiffrement_i(3)(1) xor chiffrement_i(1)(3);
	linear_tmp_s(1)(2) <= chiffrement_i(3)(2) xor chiffrement_i(1)(3);
	linear_tmp_s(1)(3) <= chiffrement_i(3)(3) xor chiffrement_i(1)(0) xor chiffrement_i(1)(1) xor chiffrement_i(1)(2);
	linear             <= chiffrement_i(0)(3) xor chiffrement_i(0)(1) xor chiffrement_i(0)(2) xor chiffrement_i(1)(3);

	linear_s(0)(0) <= non_linear_s1(0)(0) xor linear_tmp_s(0)(0);
	linear_s(0)(1) <= non_linear_s1(0)(1) xor linear_tmp_s(0)(1);
	linear_s(0)(2) <= non_linear_s1(0)(2) xor linear_tmp_s(0)(2);
	linear_s(0)(3) <= non_linear_s1(0)(3) xor linear_tmp_s(0)(3);
	linear_s(1)(0) <= non_linear_s1(1)(0) xor linear_tmp_s(1)(0);
	linear_s(1)(1) <= non_linear_s1(1)(1) xor linear_tmp_s(1)(1);
	linear_s(1)(2) <= non_linear_s1(1)(2) xor linear_tmp_s(1)(2);
	linear_s(1)(3) <= non_linear_s1(1)(3) xor linear xor linear_tmp_s(1)(3);


	chiffrement_o(0)(0) <= linear_s(1)(2)      when permutation_i='1' else chiffrement_i(0)(0);
	chiffrement_o(0)(1) <= linear_s(0)(3)      when permutation_i='1' else chiffrement_i(0)(1);
	chiffrement_o(0)(2) <= linear_s(1)(0)      when permutation_i='1' else chiffrement_i(0)(2);
	chiffrement_o(0)(3) <= linear_s(0)(2)      when permutation_i='1' else chiffrement_i(0)(3);
	chiffrement_o(1)(0) <= linear_s(0)(0)      when permutation_i='1' else chiffrement_i(1)(0);
	chiffrement_o(1)(1) <= linear_s(0)(1)      when permutation_i='1' else chiffrement_i(1)(1);
	chiffrement_o(1)(2) <= linear_s(1)(1)      when permutation_i='1' else chiffrement_i(1)(2);
	chiffrement_o(1)(3) <= linear_s(1)(3)      when permutation_i='1' else chiffrement_i(1)(3);
	chiffrement_o(2)(0) <= chiffrement_i(0)(3) when permutation_i='1' else linear_s(0)(0);
	chiffrement_o(2)(1) <= chiffrement_i(0)(1) when permutation_i='1' else linear_s(0)(1);
	chiffrement_o(2)(2) <= chiffrement_i(1)(0) when permutation_i='1' else linear_s(0)(2);
	chiffrement_o(2)(3) <= chiffrement_i(1)(1) when permutation_i='1' else linear_s(0)(3);
	chiffrement_o(3)(0) <= chiffrement_i(1)(2) when permutation_i='1' else linear_s(1)(0);
	chiffrement_o(3)(1) <= chiffrement_i(0)(0) when permutation_i='1' else linear_s(1)(1);
	chiffrement_o(3)(2) <= chiffrement_i(0)(2) when permutation_i='1' else linear_s(1)(2);
	chiffrement_o(3)(3) <= chiffrement_i(1)(3) when permutation_i='1' else linear_s(1)(3);

end chiffrement_arch;

configuration chiffrement_conf of chiffrement is
	for chiffrement_arch
		for boucle_ligne
			for boucle_colonne
				for all : sbox
					use entity work.sbox( sbox_arch );
				end for;
			end for;
		end for;
	end for;
end configuration chiffrement_conf ;
