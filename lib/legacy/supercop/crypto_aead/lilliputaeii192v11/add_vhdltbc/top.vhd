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


entity top is
	port (
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
end top;

architecture top_arch of top is

	component roundexe_liliput
		port(
			clock_i           : in  std_logic;
			reset_i           : in  std_logic;
			data_i            : in  bit_data; --donnée d'entrée lors du premier Round
			keyb_i            : in  bit_key;
			tweak_i           : in  bit_tweak;
			invert_i          : in  std_logic;
			round_number_i    : in  std_logic_vector(7 downto 0) ;
			permut_valid_i    : in  std_logic; --permet de savoir si on fait la permutation à la fin 
			mux_keyschdule_i  : in  std_logic;
			mux_chiffrement_i : in  std_logic;
			data_out_valid_i  : in  std_logic;
			data_out_valid_o  : out std_logic;
			decrypt_i         : in  std_logic;
			data_o            : out bit_data
		);
	end component;

	component fsm_chiffrement
		port (
			start_i           : in  std_logic;
			clock_i           : in  std_logic;
			reset_i           : in  std_logic;
			decrypt_i         : in  std_logic;
			compteur_o        : out std_logic_vector(7 downto 0);
			liliput_on_out    : out std_logic; --Sortie à titre informative
			data_out_valid_o  : out std_logic; --Vient à l'entrée du round exe pour s 
			permutation_o     : out std_logic;
			invert_o          : out std_logic;
			mux_keyschdule_o  : out std_logic;
			mux_chiffrement_o : out std_logic
		);
	end component;

	signal data_out_valid_s   : std_logic;
	signal permutation_o_s    : std_logic;
	signal compteur_o_s       : std_logic_vector(7 downto 0);
	signal mux_keyschdule_s   : std_logic;
	signal mux_chiffrement_s  : std_logic;
	signal invert_s           : std_logic;



begin


	machine_a_etat : fsm_chiffrement
		port map(
			start_i           => start_i,
			clock_i           => clock_i,
			reset_i           => reset_i,
			decrypt_i         => decrypt_i,
			compteur_o        => compteur_o_s,
			liliput_on_out    => liliput_on_out,
			data_out_valid_o  => data_out_valid_s,
			permutation_o     => permutation_o_s,
			invert_o          => invert_s,
			mux_keyschdule_o  => mux_keyschdule_s,
			mux_chiffrement_o => mux_chiffrement_s
		);


	roundexe_general : roundexe_liliput
		port map(
			clock_i           => clock_i,
			reset_i           => reset_i,
			data_i            => data_i,
			keyb_i            => key_i,
			tweak_i           => tweak_i,
			invert_i          => invert_s,
			round_number_i    => compteur_o_s,
			permut_valid_i    => permutation_o_s,
			mux_keyschdule_i  => mux_keyschdule_s,
			mux_chiffrement_i => mux_chiffrement_s,
			data_out_valid_i  => data_out_valid_s,
			data_out_valid_o  => valid_o,
			decrypt_i         => decrypt_i,
			data_o            => data_o
		);


end top_arch;

configuration top_conf of top is
	for top_arch
		for machine_a_etat : fsm_chiffrement
			use entity work.fsm_chiffrement(fsm_chiffrement_arch);
		end for;
		for roundexe_general : roundexe_liliput
			use entity work.roundexe_liliput(roundexe_liliput_arch);
		end for;
	end for;

end configuration top_conf;
