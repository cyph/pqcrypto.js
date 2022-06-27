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


entity multiplications is
	port (
		mularray_i : in  type_tweak_key_array;
		mularray_o : out type_tweak_key_array
	);
end multiplications;

architecture Behavioral of multiplications is

	signal x1_M_5   : bit8;
	signal x1_M_4   : bit8;
	signal x1_M_2   : bit8;
	signal x2_M_5   : bit8;
	signal x2_M_4   : bit8;
	signal x2_M_2   : bit8;
	signal x2_M2_5  : bit8;
	signal x2_M2_4  : bit8;
	signal x2_M2_2  : bit8;
	signal x3_M_5   : bit8;
	signal x3_M_4   : bit8;
	signal x3_M_2   : bit8;
	signal x3_M2_5  : bit8;
	signal x3_M2_4  : bit8;
	signal x3_M2_2  : bit8;
	signal x3_M3_4  : bit8;
	signal x3_M3_5  : bit8;
	signal x3_M3_2  : bit8;
	signal x5_MR_2  : bit8;
	signal x5_MR_4  : bit8;
	signal x5_MR_5  : bit8;
	signal x6_MR_2  : bit8;
	signal x6_MR_4  : bit8;
	signal x6_MR_5  : bit8;
	signal x6_MR2_2 : bit8;
	signal x6_MR2_4 : bit8;
	signal x6_MR2_5 : bit8;

begin

	mularray_o(0)(7) <= mularray_i(0)(6);
	mularray_o(0)(6) <= mularray_i(0)(5);
	mularray_o(0)(5) <= std_logic_vector(shift_left(unsigned(mularray_i(0)(5)), 3)) xor mularray_i(0)(4);
	mularray_o(0)(4) <= std_logic_vector(shift_right(unsigned(mularray_i(0)(4)), 3)) xor mularray_i(0)(3);
	mularray_o(0)(3) <= mularray_i(0)(2);
	mularray_o(0)(2) <= std_logic_vector(shift_left(unsigned(mularray_i(0)(6)) , 2)) xor mularray_i(0)(1);
	mularray_o(0)(1) <= mularray_i(0)(0);
	mularray_o(0)(0) <= mularray_i(0)(7);

	x1_M_5 <= std_logic_vector(shift_left(unsigned(mularray_i(1)(5)), 3)) xor mularray_i(1)(4);
	x1_M_4 <= std_logic_vector(shift_right(unsigned(mularray_i(1)(4)), 3)) xor mularray_i(1)(3);
	x1_M_2 <= std_logic_vector(shift_left(unsigned(mularray_i(1)(6)), 2)) xor mularray_i(1)(1);

	mularray_o(1)(7) <= mularray_i(1)(5);
	mularray_o(1)(6) <= x1_M_5;
	mularray_o(1)(5) <= std_logic_vector(shift_left(unsigned(x1_M_5), 3)) xor x1_M_4;
	mularray_o(1)(4) <= std_logic_vector(shift_right(unsigned(x1_M_4), 3)) xor mularray_i(1)(2);
	mularray_o(1)(3) <= x1_M_2;
	mularray_o(1)(2) <= std_logic_vector(shift_left(unsigned(mularray_i(1)(5)), 2)) xor mularray_i(1)(0);
	mularray_o(1)(1) <= mularray_i(1)(7);
	mularray_o(1)(0) <= mularray_i(1)(6);

	x2_M_5  <= std_logic_vector(shift_left(unsigned(mularray_i(2)(5)), 3)) xor mularray_i(2)(4);
	x2_M_4  <= std_logic_vector(shift_right(unsigned(mularray_i(2)(4)), 3)) xor mularray_i(2)(3);
	x2_M_2  <= std_logic_vector(shift_left(unsigned(mularray_i(2)(6)), 2)) xor mularray_i(2)(1);
	x2_M2_5 <= std_logic_vector(shift_left(unsigned(x2_M_5), 3)) xor x2_M_4;
	x2_M2_4 <= std_logic_vector(shift_right(unsigned(x2_M_4), 3)) xor mularray_i(2)(2);
	x2_M2_2 <= std_logic_vector(shift_left(unsigned(mularray_i(2)(5)), 2)) xor mularray_i(2)(0);

	mularray_o(2)(7) <= x2_M_5;
	mularray_o(2)(6) <= x2_M2_5;
	mularray_o(2)(5) <= std_logic_vector(shift_left(unsigned(x2_M2_5) , 3)) xor x2_M2_4;
	mularray_o(2)(4) <= std_logic_vector(shift_right(unsigned(x2_M2_4), 3)) xor x2_M_2;
	mularray_o(2)(3) <= x2_M2_2;
	mularray_o(2)(2) <= std_logic_vector(shift_left(unsigned(x2_M_5) , 2)) xor mularray_i(2)(7);
	mularray_o(2)(1) <= mularray_i(2)(6);
	mularray_o(2)(0) <= mularray_i(2)(5);

	x3_M_5  <= std_logic_vector(shift_left(unsigned(mularray_i(3)(5)), 3)) xor mularray_i(3)(4);
	x3_M_4  <= std_logic_vector(shift_right(unsigned(mularray_i(3)(4)), 3)) xor mularray_i(3)(3);
	x3_M_2  <= std_logic_vector(shift_left(unsigned(mularray_i(3)(6)), 2)) xor mularray_i(3)(1);
	x3_M2_5 <= std_logic_vector(shift_left(unsigned(x3_M_5), 3)) xor x3_M_4;
	x3_M2_4 <= std_logic_vector(shift_right(unsigned(x3_M_4), 3)) xor mularray_i(3)(2);
	x3_M2_2 <= std_logic_vector(shift_left(unsigned(mularray_i(3)(5)), 2)) xor mularray_i(3)(0);
	x3_M3_4 <= std_logic_vector(shift_right(unsigned(x3_M2_4), 3)) xor x3_M_2;
	x3_M3_5 <= std_logic_vector(shift_left(unsigned(x3_M2_5), 3)) xor x3_M2_4;
	x3_M3_2 <= std_logic_vector(shift_left(unsigned(x3_M_5), 2)) xor mularray_i(3)(7);

	mularray_o(3)(7) <= x3_M2_5;
	mularray_o(3)(6) <= x3_M3_5;
	mularray_o(3)(5) <= std_logic_vector(shift_left(unsigned(x3_M3_5) , 3)) xor x3_M3_4;
	mularray_o(3)(4) <= std_logic_vector(shift_right(unsigned(x3_M3_4), 3)) xor x3_M2_2;
	mularray_o(3)(3) <= x3_M3_2;
	mularray_o(3)(2) <= std_logic_vector(shift_left(unsigned(x3_M2_5) , 2)) xor mularray_i(3)(6);
	mularray_o(3)(1) <= mularray_i(3)(5);
	mularray_o(3)(0) <= x3_M_5;


	if_lane5_6_7 : if LANE_NB>4 generate
		mularray_o(4)(0) <= mularray_i(4)(1);
		mularray_o(4)(1) <= mularray_i(4)(2);
		mularray_o(4)(2) <= mularray_i(4)(3)xor std_logic_vector(shift_right(unsigned(mularray_i(4)(4)), 3));
		mularray_o(4)(3) <= mularray_i(4)(4);
		mularray_o(4)(4) <= mularray_i(4)(5) xor std_logic_vector(shift_left(unsigned(mularray_i(4)(6)) , 3));
		mularray_o(4)(5) <= mularray_i(4)(6) xor std_logic_vector(shift_left(unsigned(mularray_i(4)(3)) , 2));
		mularray_o(4)(6) <= mularray_i(4)(7);
		mularray_o(4)(7) <= mularray_i(4)(0);
	end generate;

	if_lane6_7 : if LANE_NB>5 generate
		x5_MR_2 <= mularray_i(5)(3) xor std_logic_vector(shift_right(unsigned(mularray_i(5)(4)) , 3));
		x5_MR_4 <= mularray_i(5)(5) xor std_logic_vector(shift_left(unsigned(mularray_i(5)(6)) , 3));
		x5_MR_5 <= mularray_i(5)(6) xor std_logic_vector(shift_left(unsigned(mularray_i(5)(3)) , 2));

		mularray_o(5)(0) <= mularray_i(5)(2);
		mularray_o(5)(1) <= x5_MR_2;
		mularray_o(5)(2) <= mularray_i(5)(4) xor std_logic_vector(shift_right(unsigned(x5_MR_4) , 3));
		mularray_o(5)(3) <= x5_MR_4;
		mularray_o(5)(4) <= x5_MR_5 xor std_logic_vector(shift_left(unsigned(mularray_i(5)(7)) , 3));
		mularray_o(5)(5) <= mularray_i(5)(7) xor std_logic_vector(shift_left(unsigned(mularray_i(5)(4)) , 2));
		mularray_o(5)(6) <= mularray_i(5)(0);
		mularray_o(5)(7) <= mularray_i(5)(1);
	end generate;

	if_lane7 : if LANE_NB>6 generate
		x6_MR_2  <= mularray_i(6)(3) xor std_logic_vector(shift_right(unsigned(mularray_i(6)(4)) , 3));
		x6_MR_4  <= mularray_i(6)(5) xor std_logic_vector(shift_left(unsigned(mularray_i(6)(6)) , 3));
		x6_MR_5  <= mularray_i(6)(6) xor std_logic_vector(shift_left(unsigned(mularray_i(6)(3)) , 2));
		x6_MR2_2 <= mularray_i(6)(4) xor std_logic_vector(shift_right(unsigned(x6_MR_4) , 3));
		x6_MR2_4 <= x6_MR_5 xor std_logic_vector(shift_left(unsigned(mularray_i(6)(7)) , 3));
		x6_MR2_5 <= mularray_i(6)(7) xor std_logic_vector(shift_left(unsigned(mularray_i(6)(4)) , 2));

		mularray_o(6)(0) <= x6_MR_2;
		mularray_o(6)(1) <= x6_MR2_2;
		mularray_o(6)(2) <= x6_MR_4 xor std_logic_vector(shift_right(unsigned(x6_MR2_4) , 3));
		mularray_o(6)(3) <= x6_MR2_4;
		mularray_o(6)(4) <= x6_MR2_5 xor std_logic_vector(shift_left(unsigned(mularray_i(6)(0)) , 3));
		mularray_o(6)(5) <= mularray_i(6)(0) xor std_logic_vector(shift_left(unsigned(x6_MR_4) , 2));
		mularray_o(6)(6) <= mularray_i(6)(1);
		mularray_o(6)(7) <= mularray_i(6)(2);
	end generate;

end Behavioral;
