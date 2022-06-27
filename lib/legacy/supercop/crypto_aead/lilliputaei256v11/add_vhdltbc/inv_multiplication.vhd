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


entity inv_multiplication is
    port (
        mularray_i : in  type_tweak_key_array;
        mularray_o : out type_tweak_key_array
    );
end inv_multiplication;

architecture inv_multiplication_arch of inv_multiplication is

    signal x1_M_1   : bit8;
    signal x1_M_3   : bit8;
    signal x1_M_4   : bit8;
    signal x2_M_1   : bit8;
    signal x2_M_3   : bit8;
    signal x2_M_4   : bit8;
    signal x2_M2_1  : bit8;
    signal x2_M2_3  : bit8;
    signal x2_M2_4  : bit8;
    signal x3_M_1   : bit8;
    signal x3_M_3   : bit8;
    signal x3_M_4   : bit8;
    signal x3_M2_1  : bit8;
    signal x3_M2_3  : bit8;
    signal x3_M2_4  : bit8;
    signal x3_M3_1  : bit8;
    signal x3_M3_3  : bit8;
    signal x3_M3_4  : bit8;
    signal x5_MR_3  : bit8;
    signal x5_MR_5  : bit8;
    signal x5_MR_6  : bit8;
    signal x6_MR_3  : bit8;
    signal x6_MR_5  : bit8;
    signal x6_MR_6  : bit8;
    signal x6_MR2_3 : bit8;
    signal x6_MR2_5 : bit8;
    signal x6_MR2_6 : bit8;

begin

    mularray_o(0)(7) <= mularray_i(0)(0);
    mularray_o(0)(6) <= mularray_i(0)(7);
    mularray_o(0)(5) <= mularray_i(0)(6);
    mularray_o(0)(4) <= mularray_i(0)(5)xor std_logic_vector(shift_left(unsigned(mularray_i(0)(6)) , 3));
    mularray_o(0)(3) <= mularray_i(0)(4)xor std_logic_vector(shift_right(unsigned(mularray_i(0)(5)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(0)(6)) , 3))) , 3));
    mularray_o(0)(2) <= mularray_i(0)(3);
    mularray_o(0)(1) <= mularray_i(0)(2) xor std_logic_vector(shift_left(unsigned(mularray_i(0)(7)) , 2));
    mularray_o(0)(0) <= mularray_i(0)(1);

    x1_M_4 <= mularray_i(1)(5)xor std_logic_vector(shift_left(unsigned(mularray_i(1)(6)) , 3));
    x1_M_3 <= mularray_i(1)(4)xor std_logic_vector(shift_right(unsigned(mularray_i(1)(5)) , 3))xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(1)(6)) , 3))) , 3));
    x1_M_1 <= mularray_i(1)(2) xor std_logic_vector(shift_left(unsigned(mularray_i(1)(7)) , 2));


    mularray_o(1)(7) <= mularray_i(1)(1);
    mularray_o(1)(6) <= mularray_i(1)(0);
    mularray_o(1)(5) <= mularray_i(1)(7);
    mularray_o(1)(4) <= mularray_i(1)(6)xor std_logic_vector(shift_left(unsigned(mularray_i(1)(7)) , 3));
    mularray_o(1)(3) <= x1_M_4 xor std_logic_vector(shift_right(unsigned(mularray_i(1)(6)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(1)(7)) , 3))) , 3));
    mularray_o(1)(2) <= x1_M_3;
    mularray_o(1)(1) <= mularray_i(1)(3) xor std_logic_vector(shift_left(unsigned(mularray_i(1)(0)) , 2));
    mularray_o(1)(0) <= x1_M_1;

    x2_M_4  <= mularray_i(2)(5)xor std_logic_vector(shift_left(unsigned(mularray_i(2)(6)) , 3));
    x2_M_3  <= mularray_i(2)(4)xor std_logic_vector(shift_right(unsigned(mularray_i(2)(5)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(2)(6)) , 3))) , 3));
    x2_M_1  <= mularray_i(2)(2) xor std_logic_vector(shift_left(unsigned(mularray_i(2)(7)) , 2));
    x2_M2_4 <= mularray_i(2)(6)xor std_logic_vector(shift_left(unsigned(mularray_i(2)(7)) , 3));
    x2_M2_3 <= x2_M_4 xor std_logic_vector(shift_right(unsigned(mularray_i(2)(6)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(2)(7)) , 3))) , 3));
    x2_M2_1 <= mularray_i(2)(3) xor std_logic_vector(shift_left(unsigned(mularray_i(2)(0)) , 2));

    mularray_o(2)(7) <= x2_M_1;
    mularray_o(2)(6) <= mularray_i(2)(1);
    mularray_o(2)(5) <= mularray_i(2)(0);
    mularray_o(2)(4) <= mularray_i(2)(7)xor std_logic_vector(shift_left(unsigned(mularray_i(2)(0)) , 3));
    mularray_o(2)(3) <= x2_M2_4 xor std_logic_vector(shift_right(unsigned(mularray_i(2)(7)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(2)(0)) , 3))) , 3));
    mularray_o(2)(2) <= x2_M2_3;
    mularray_o(2)(1) <= x2_M_3 xor std_logic_vector(shift_left(unsigned(mularray_i(2)(1)) , 2));
    mularray_o(2)(0) <= x2_M2_1;

    x3_M_4  <= mularray_i(3)(5)xor std_logic_vector(shift_left(unsigned(mularray_i(3)(6)) , 3));
    x3_M_3  <= mularray_i(3)(4)xor std_logic_vector(shift_right(unsigned(mularray_i(3)(5)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(3)(6)) , 3))) , 3));
    x3_M_1  <= mularray_i(3)(2) xor std_logic_vector(shift_left(unsigned(mularray_i(3)(7)) , 2));
    x3_M2_4 <= mularray_i(3)(6)xor std_logic_vector(shift_left(unsigned(mularray_i(3)(7)) , 3));
    x3_M2_3 <= x3_M_4 xor std_logic_vector(shift_right(unsigned(mularray_i(3)(6)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(3)(7)) , 3))) , 3));
    x3_M2_1 <= mularray_i(3)(3) xor std_logic_vector(shift_left(unsigned(mularray_i(3)(0)) , 2));
    x3_M3_4 <= mularray_i(3)(7)xor std_logic_vector(shift_left(unsigned(mularray_i(3)(0)) , 3));
    x3_M3_3 <= x3_M2_4 xor std_logic_vector(shift_right(unsigned(mularray_i(3)(7)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(3)(0)) , 3))) , 3));
    x3_M3_1 <= x3_M_3 xor std_logic_vector(shift_left(unsigned(mularray_i(3)(1)) , 2));

    mularray_o(3)(7) <= x3_M2_1;
    mularray_o(3)(6) <= x3_M_1;
    mularray_o(3)(5) <= mularray_i(3)(1);
    mularray_o(3)(4) <= mularray_i(3)(0)xor std_logic_vector(shift_left(unsigned(mularray_i(3)(1)) , 3));
    mularray_o(3)(3) <= x3_M3_4 xor std_logic_vector(shift_right(unsigned(mularray_i(3)(0)) , 3)) xor std_logic_vector(shift_right(unsigned(std_logic_vector(shift_left(unsigned(mularray_i(3)(1)) , 3))) , 3));
    mularray_o(3)(2) <= x3_M3_3;
    mularray_o(3)(1) <= x3_M2_3 xor std_logic_vector(shift_left(unsigned(x3_M_1) , 2));
    mularray_o(3)(0) <= x3_M3_1;


    if_lane5_6_7 : if LANE_NB>4 generate
        mularray_o(4)(0) <= mularray_i(4)(7);
        mularray_o(4)(1) <= mularray_i(4)(0);
        mularray_o(4)(2) <= mularray_i(4)(1);
        mularray_o(4)(3) <= mularray_i(4)(2) xor std_logic_vector(shift_right(unsigned(mularray_i(4)(3)), 3));
        mularray_o(4)(4) <= mularray_i(4)(3);
        mularray_o(4)(5) <= mularray_i(4)(4) xor std_logic_vector(shift_left(unsigned(mularray_i(4)(2)) , 5)) xor std_logic_vector(shift_left(shift_right(unsigned(mularray_i(4)(3)) , 3) , 5)) xor std_logic_vector(shift_left(unsigned(mularray_i(4)(5)) , 3));
        mularray_o(4)(6) <= mularray_i(4)(5) xor std_logic_vector(shift_left(unsigned(mularray_i(4)(2)) , 2)) xor std_logic_vector(shift_left(shift_right(unsigned(mularray_i(4)(3)) , 3) , 2));
        mularray_o(4)(7) <= mularray_i(4)(6);
    end generate;

    if_lane6_7 : if LANE_NB>5 generate
        x5_MR_3 <= mularray_i(5)(2) xor std_logic_vector(shift_right(unsigned(mularray_i(5)(3)), 3));
        x5_MR_5 <= mularray_i(5)(4) xor std_logic_vector(shift_left(unsigned(mularray_i(5)(2)) , 5)) xor std_logic_vector(shift_left(shift_right(unsigned(mularray_i(5)(3)) , 3) , 5)) xor std_logic_vector(shift_left(unsigned(mularray_i(5)(5)) , 3));
        x5_MR_6 <= mularray_i(5)(5) xor std_logic_vector(shift_left(unsigned(mularray_i(5)(2)) , 2)) xor std_logic_vector(shift_left(shift_right(unsigned(mularray_i(5)(3)) , 3) , 2));

        mularray_o(5)(0) <= mularray_i(5)(6);
        mularray_o(5)(1) <= mularray_i(5)(7);
        mularray_o(5)(2) <= mularray_i(5)(0);
        mularray_o(5)(3) <= mularray_i(5)(1) xor std_logic_vector(shift_right(unsigned(x5_MR_3), 3));
        mularray_o(5)(4) <= x5_MR_3;
        mularray_o(5)(5) <= mularray_i(5)(3) xor std_logic_vector(shift_left(unsigned(mularray_i(5)(1)) , 5)) xor std_logic_vector(shift_left(shift_right(unsigned(x5_MR_3) , 3) , 5)) xor std_logic_vector(shift_left(unsigned(x5_MR_5) , 3));
        mularray_o(5)(6) <= x5_MR_5 xor std_logic_vector(shift_left(unsigned(mularray_i(5)(1)) , 2)) xor std_logic_vector(shift_left(shift_right(unsigned(x5_MR_3) , 3) , 2));
        mularray_o(5)(7) <= x5_MR_6;
    end generate;

    if_lane7 : if LANE_NB>6 generate
        x6_MR_3  <= mularray_i(6)(2) xor std_logic_vector(shift_right(unsigned(mularray_i(6)(3)), 3));
        x6_MR_5  <= mularray_i(6)(4) xor std_logic_vector(shift_left(unsigned(mularray_i(6)(2)) , 5)) xor std_logic_vector(shift_left(shift_right(unsigned(mularray_i(6)(3)) , 3) , 5)) xor std_logic_vector(shift_left(unsigned(mularray_i(6)(5)) , 3));
        x6_MR_6  <= mularray_i(6)(5) xor std_logic_vector(shift_left(unsigned(mularray_i(6)(2)) , 2)) xor std_logic_vector(shift_left(shift_right(unsigned(mularray_i(6)(3)) , 3) , 2));
        x6_MR2_3 <= mularray_i(6)(1) xor std_logic_vector(shift_right(unsigned(x6_MR_3), 3));
        x6_MR2_5 <= mularray_i(6)(3) xor std_logic_vector(shift_left(unsigned(mularray_i(6)(1)) , 5)) xor std_logic_vector(shift_left(shift_right(unsigned(x6_MR_3) , 3) , 5)) xor std_logic_vector(shift_left(unsigned(x6_MR_5) , 3));
        x6_MR2_6 <= x6_MR_5 xor std_logic_vector(shift_left(unsigned(mularray_i(6)(1)) , 2)) xor std_logic_vector(shift_left(shift_right(unsigned(x6_MR_3) , 3) , 2));

        mularray_o(6)(0) <= x6_MR_6;
        mularray_o(6)(1) <= mularray_i(6)(6);
        mularray_o(6)(2) <= mularray_i(6)(7);
        mularray_o(6)(3) <= mularray_i(6)(0) xor std_logic_vector(shift_right(unsigned(x6_MR2_3), 3));
        mularray_o(6)(4) <= x6_MR2_3;
        mularray_o(6)(5) <= x6_MR_3 xor std_logic_vector(shift_left(unsigned(mularray_i(6)(0)) , 5)) xor std_logic_vector(shift_left(shift_right(unsigned(x6_MR2_3) , 3) , 5)) xor std_logic_vector(shift_left(unsigned(x6_MR2_5) , 3));
        mularray_o(6)(6) <= x6_MR2_5 xor std_logic_vector(shift_left(unsigned(mularray_i(6)(0)) , 2)) xor std_logic_vector(shift_left(shift_right(unsigned(x6_MR2_3) , 3) , 2));
        mularray_o(6)(7) <= x6_MR2_6;
    end generate;

end inv_multiplication_arch;