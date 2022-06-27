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

entity roundexe_liliput is
	port (
		clock_i           : in  std_logic;
		reset_i           : in  std_logic;
		data_i            : in  bit_data; --donnée d'entrée lors du premier Round
		keyb_i            : in  bit_key;
		tweak_i           : in  bit_tweak;
		invert_i          : in  std_logic;
		round_number_i    : in  std_logic_vector(7 downto 0);
		permut_valid_i    : in  std_logic; --permet de savoir si on fait la permutation à la fin 
		mux_keyschdule_i  : in  std_logic;
		mux_chiffrement_i : in  std_logic;
		data_out_valid_i  : in  std_logic;
		data_out_valid_o  : out std_logic;
		decrypt_i         : in  std_logic;
		data_o            : out bit_data
	);
end roundexe_liliput;

architecture roundexe_liliput_arch of roundexe_liliput is

	component chiffrement
		port(
			chiffrement_i : in  type_state;
			permutation_i : in  std_logic;
			round_key_i   : in  type_key;
			chiffrement_o : out type_state
		);
	end component;

	component key_schedule_liliput
		port (
			key_i        : in  type_tweak_key_array;
			round_number : in  std_logic_vector(7 downto 0);
			key_o        : out type_tweak_key_array;
			round_key_o  : out type_key
		);
	end component;


	signal data_i_s          : type_state;
	signal chiffrement_o_s   : type_state;
	signal mux_1_s           : type_state;
	signal mux_2_s           : type_tweak_key_array;
	signal state_o_s         : type_state;
	signal state_tk_o_s      : type_tweak_key_array;
	signal round_key_o_s     : type_key;
	signal state_round_key_s : type_key;
	signal tweak_key_i       : bit_tweak_key := (others => '0');
	signal tk_s              : type_tweak_key_array;
	signal tk_o_s            : type_tweak_key_array;
	signal data_out_valid_s  : std_logic;

begin

	convertion_ligne : for i in 0 to 3 generate
		convertion_colonne : for j in 0 to 3 generate
			data_i_s(i)(j)(7 downto 4)                       <= data_i((3+(8*(4*i+j)))downto((8*(4*i+j))));
			data_i_s(i)(j)(3 downto 0)                       <= data_i((7+(8*(4*i+j)))downto(4+(8*(4*i+j))));
			data_o(7+(8*(4*i+j)) downto 4+(8*(4*i+j)))       <= state_o_s(i)(j)(3 downto 0) when data_out_valid_s = '1' else X"0";
			data_o(3+(8*(4*i+j)) downto (8*(4*i+j)))         <= state_o_s(i)(j)(7 downto 4)  when data_out_valid_s = '1' else X"0";
		end generate;
	end generate;

	data_out_valid_o <= data_out_valid_s;

		reg_roundkey : process(reset_i, clock_i)
	begin
		if(reset_i = '0') then
			state_round_key_s <= (others => (others => (others => '0')));
			state_tk_o_s      <= (others => (others => (others => '0')));
			state_o_s         <= (others => (others => (others => '0')));
			data_out_valid_s  <= '0';

		elsif(clock_i'event and clock_i = '1') then
			state_round_key_s <= round_key_o_s;
			state_tk_o_s      <= tk_o_s;
			state_o_s         <= mux_1_s;
			data_out_valid_s  <= data_out_valid_i;
		end if;
	end process reg_roundkey;

	--Tweak_key concatenation
	tweak_key_i (TWEAK_KEY_LEN downto 0) <= keyb_i & tweak_i;

	--formatting tweak_key in type_tweak_key_array
	convertion_ligne_key : for i in 0 to LANE_NB-1 generate
		convertion_colonne_key : for j in 0 to 7 generate
			tk_s(i)(j)(7 downto 4) <= tweak_key_i(((64*i)+(8*j)+3)downto((64*i)+(8*j)));
			tk_s(i)(j)(3 downto 0) <= tweak_key_i(((64*i)+(8*j)+7)downto((64*i)+(8*j)+4));
		end generate;
	end generate;

	--Avantage on n'utilise le même mux donc pas de changement dans la machine d'état
	mux_1_s <= data_i_s when mux_chiffrement_i = '1' else
		chiffrement_o_s;

	mux_2_s <= tk_s when mux_keyschdule_i = '1' else
		state_tk_o_s;

		key_schedule_t : key_schedule_liliput port map(
			key_i        => mux_2_s,
			round_number => round_number_i,
			key_o        => tk_o_s,
			round_key_o  => round_key_o_s
		);

		chiffrement_t : chiffrement port map(
			chiffrement_i => state_o_s,
			permutation_i => permut_valid_i,
			round_key_i   => state_round_key_s,
			chiffrement_o => chiffrement_o_s
		);

end roundexe_liliput_arch;

configuration roundexe_liliput_conf of roundexe_liliput is
	for roundexe_liliput_arch
		for key_schedule_t : key_schedule_liliput
			use entity work.key_schedule_liliput(key_schedule_liliputr_arch);
		end for;
		for chiffrement_t : chiffrement
			use entity work.chiffrement(chiffrement_arch);
		end for;
	end for;
end configuration roundexe_liliput_conf;