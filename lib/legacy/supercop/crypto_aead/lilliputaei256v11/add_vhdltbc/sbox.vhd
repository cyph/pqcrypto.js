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
use IEEE.std_logic_1164.all;
use work.crypt_pack.all;

entity sbox is
	port(
		sbox_i : in  bit8;
		sbox_o : out bit8
	);
end sbox;



architecture sbox_arch of sbox is

	signal a,b : std_logic_vector(3 downto 0);

	signal ax,ay,at,az : std_logic;
	signal aa,ab       : std_logic;

	signal bx,bz : std_logic;
	signal bt,by : std_logic;

	signal cx,cy,ct : std_logic;
	signal ca,cb,cz : std_logic;

begin


	aa <= sbox_i(3) xor sbox_i(1);
	ab <= sbox_i(0) xor (sbox_i(2) and sbox_i(1));

	az <= sbox_i(2) xor ab;
	ax <= aa and (sbox_i(2) xor ab);
	ay <= sbox_i(3) and ab;
	at <= (az xor sbox_i(3)) and (sbox_i(2) xor aa);

	a <= ax & ay & az & at xor sbox_i(7 downto 4);

	bx <= a(0) xor (a(3) and by);
	bz <= a(3) xor (bt and by);
	by <= a(2) xor (a(0) and a(1));
	bt <= a(1) xor (a(3) and a(0));

	b <= bx & by & bz & bt xor sbox_i(3 downto 0);

	ca <= b(3) xor b(1);
	cb <= not (b(0) xor (b(2) and b(1)));

	cx <= ca and cz;
	cz <= b(2) xor cb;
	cy <= b(3) and cb;
	ct <= (cz xor b(3)) and (b(2) xor ca);

	sbox_o (7 downto 4) <= cx & cy & cz & ct xor a;
	sbox_o (3 downto 0) <= b;

end sbox_arch;

