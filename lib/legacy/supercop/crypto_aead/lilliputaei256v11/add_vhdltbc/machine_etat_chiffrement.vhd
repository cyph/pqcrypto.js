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
		compteur_o        : out std_logic_vector(7 downto 0) ;
		liliput_on_out    : out std_logic;
		data_out_valid_o  : out std_logic;
		permutation_o     : out std_logic;
		invert_o          : out std_logic;
		mux_keyschdule_o  : out std_logic;
		mux_chiffrement_o : out std_logic
	);
end fsm_chiffrement;

architecture fsm_chiffrement_arch of fsm_chiffrement is

	type state is (etat_initial,initroundkey, e_firstround, e_loopround, d_initfirst,d_initloop,d_initlast,d_firstround, d_loopround, lastround);

	signal present, futur : state;
	signal compteur       : integer range 0 to ROUND+1;

begin

	compteur_o <= std_logic_vector(to_unsigned(compteur,8));

	process_0 : process(clock_i,reset_i)
	begin
		if reset_i = '0' then
			present  <= etat_initial;
			compteur <= 0;

		elsif clock_i'event and clock_i ='1' then
			present <= futur;
			if( present =d_loopround or present =d_firstround or present =d_initlast) then
				compteur <= compteur -1;
			elsif (present = initroundkey or present =d_initloop or present =d_initfirst or present = e_firstround or present =e_loopround ) then
				compteur <= compteur+1;
			else
				compteur <= 0;
			end if;
		end if;
	end process process_0;

	process_1 : process(present, start_i,decrypt_i,compteur)
	begin

		case present is
			when etat_initial =>
				if start_i = '1' then
					futur <= initroundkey;
				else
					futur <= present;
				end if;

			when initroundkey =>
				if decrypt_i = '0' then
					futur <= e_loopround;
				elsif decrypt_i = '1' then
					futur <= d_initloop;
				end if;

			when e_firstround =>
				futur <= e_loopround;

			when e_loopround =>
				if compteur = ROUND-1 then
					futur <= lastround;
				else
					futur <= present;
				end if;

			when d_initfirst =>
				futur <= d_initloop;

			when d_initloop =>
				if compteur = ROUND-2 then
					futur <= d_initlast;
				else
					futur <= present;
				end if;

			when d_initlast =>
				futur <= d_firstround;

			when d_firstround =>
				futur <= d_loopround;

			when d_loopround =>
				if compteur = 0 then
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
				invert_o          <= '0';

			when initroundkey =>
				liliput_on_out    <= '0';
				data_out_valid_o  <= '0';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '1';
				mux_chiffrement_o <= '1';
				invert_o          <= '0';

			when e_firstround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '0';
				permutation_o     <= '1';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';
				invert_o          <= '0';

			when e_loopround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '0';
				permutation_o     <= '1';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';
				invert_o          <= '0';

			when d_initfirst =>
				liliput_on_out    <= '0';
				data_out_valid_o  <= '0';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '1';
				invert_o          <= '0';

			when d_initloop =>
				liliput_on_out    <= '0';
				data_out_valid_o  <= '0';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '1';
				invert_o          <= '0';

			when d_initlast =>
				liliput_on_out    <= '0';
				data_out_valid_o  <= '0';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '1';
				invert_o          <= '1';

			when d_firstround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '0';
				permutation_o     <= '1';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';
				invert_o          <= '1';

			when d_loopround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '0';
				permutation_o     <= '1';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';
				invert_o          <= '1';

			when lastround =>
				liliput_on_out    <= '1';
				data_out_valid_o  <= '1';
				permutation_o     <= '0';
				mux_keyschdule_o  <= '0';
				mux_chiffrement_o <= '0';
				invert_o          <= '0';

		end case;
	end process process_2;

end architecture fsm_chiffrement_arch;