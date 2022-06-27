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

entity fsm_chiffrement is
	port (
		start_i           : in  std_logic;
		clock_i           : in  std_logic;
		reset_i           : in  std_logic;
		decrypt_i         : in  std_logic;
		compteur_o        : out std_logic_vector(7 downto 0);
		liliput_on_out    : out std_logic;
		invert_o          : out std_logic;
		data_out_valid_o  : out std_logic;
		permutation_o     : out std_logic;
		mux_keyschdule_o  : out std_logic;
		mux_chiffrement_o : out std_logic
	);
end fsm_chiffrement;

architecture fsm_chiffrement_arch of fsm_chiffrement is

	type state is (etat_initial,initroundkey, firstround, loopround, lastround);

	signal present, futur : state;
	signal compteur       : integer range 0 to ROUND;

begin

	invert_o   <= '0';
	compteur_o <= std_logic_vector(to_unsigned(compteur,8));

	process_0 : process(clock_i,reset_i,compteur)
	begin
		if reset_i = '0' then
			compteur <= 0;
			present  <= etat_initial;
		elsif clock_i'event and clock_i='1' then
			present <= futur;
			if (present = initroundkey or present = firstround or present =loopround) then
				compteur <= compteur+1;
			else
				compteur <= 0;
			end if;
		end if;
	end process process_0;


	process_1 : process(present, start_i, compteur)
	begin
		case present is
			when etat_initial =>
				if start_i = '1' then
					futur <= initroundkey;
				else
					futur <= present;
				end if;

			when initroundkey =>
				futur <= firstround;

			when firstround =>
				futur <= loopround;

			when loopround =>
				if compteur = ROUND-1 then
					futur <= lastround;
				else
					futur <= present;
				end if;

			when lastround =>
				futur <= etat_initial;

		end case;
	end process process_1;

	process_2 : process(present)
	begin
		case present is
			when etat_initial =>
				liliput_on_out    <= '0';
				data_out_valid_o  <= '0';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '1';
				mux_chiffrement_o <= '1';

			when initroundkey =>
				liliput_on_out    <= '0';
				data_out_valid_o  <= '0';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '1';
				mux_chiffrement_o <= '1';

			when firstround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '0';
				permutation_o     <= '1';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';

			when loopround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '0';
				permutation_o     <= '1';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';

			when lastround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '1';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';
		end case;
	end process process_2;

end architecture fsm_chiffrement_arch;