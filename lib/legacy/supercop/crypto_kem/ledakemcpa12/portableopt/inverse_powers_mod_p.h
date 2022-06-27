/**
 *
 * Optimized ISO-C11 Implementation of LEDAcrypt using GCC built-ins.
 *
 * @version 3.0 (May 2020)
 *
 * In alphabetical order:
 *
 * @author Marco Baldi <m.baldi@univpm.it>
 * @author Alessandro Barenghi <alessandro.barenghi@polimi.it>
 * @author Franco Chiaraluce <f.chiaraluce@univpm.it>
 * @author Gerardo Pelosi <gerardo.pelosi@polimi.it>
 * @author Paolo Santini <p.santini@pm.univpm.it>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

#if (P==7187)
unsigned int inverse_factors_table[13] = {
3594,
1797,
2246,
6429,
6791,
5889,
3046,
6886,
4357,
2582,
4375,
1644,
424};
#endif


#if (P==8237)
unsigned int inverse_factors_table[14] = {
4119,
6178,
5663,
2928,
6704,
2544,
5891,
1400,
7831,
96,
979,
2949,
6566,
8135};
#endif


#if (P==10853)
unsigned int inverse_factors_table[14] = {
5427,
8140,
2035,
6232,
5790,
10036,
5456,
9010,
10513,
7070,
6835,
5913,
6056,
2849};
#endif


#if (P==13109)
unsigned int inverse_factors_table[14] = {
6555,
9832,
2458,
11624,
2913,
4046,
10084,
543,
6451,
7435,
11681,
7289,
11853,
4456};
#endif


#if (P==13397)
unsigned int inverse_factors_table[14] = {
6699,
10048,
2512,
157,
11252,
5854,
13187,
3909,
7701,
10279,
9099,
11738,
5896,
10998};
#endif


#if (P==15331)
unsigned int inverse_factors_table[14] = {
7666,
3833,
4791,
3174,
1809,
6978,
1228,
5546,
4130,
8828,
6111,
13336,
9296,
10100};
#endif


#if (P==16067)
unsigned int inverse_factors_table[14] = {
8034,
4017,
5021,
1318,
1888,
13737,
14321,
11853,
3761,
6161,
7667,
9803,
2082,
12701};
#endif


#if (P==16229)
unsigned int inverse_factors_table[14] = {
8115,
12172,
3043,
9319,
2382,
10003,
8224,
7933,
12656,
10335,
9176,
2924,
13322,
11569};
#endif


#if (P==19709)
unsigned int inverse_factors_table[15] = {
9855,
14782,
13550,
13165,
15988,
10123,
8038,
3342,
13670,
7871,
7254,
17195,
13316,
13692,
18565};
#endif


#if (P==20981)
unsigned int inverse_factors_table[15] = {
10491,
15736,
3934,
13359,
19476,
20058,
12689,
2527,
7505,
12021,
8294,
14718,
11680,
3938,
2885};
#endif


#if (P==21611)
unsigned int inverse_factors_table[15] = {
10806,
5403,
17559,
15955,
6056,
1269,
11147,
13970,
13570,
19180,
9958,
10496,
14749,
18286,
12404};
#endif


#if (P==22901)
unsigned int inverse_factors_table[15] = {
11451,
17176,
4294,
3131,
1533,
14187,
16981,
7870,
12596,
1088,
15793,
4058,
1545,
5321,
7405};
#endif


#if (P==23371)
unsigned int inverse_factors_table[15] = {
11686,
5843,
18989,
14333,
3799,
12494,
5127,
17125,
6317,
10192,
16140,
6434,
6315,
8299,
22435};
#endif


#if (P==25579)
unsigned int inverse_factors_table[15] = {
12790,
6395,
20783,
6095,
8317,
6873,
19295,
20259,
12026,
1010,
22519,
1686,
3327,
18801,
1400};
#endif


#if (P==28277)
unsigned int inverse_factors_table[15] = {
14139,
21208,
5302,
3866,
15700,
27668,
3280,
13140,
238,
90,
8100,
7360,
19145,
4551,
12837};
#endif


#if (P==28411)
unsigned int inverse_factors_table[15] = {
14206,
7103,
23084,
22751,
16403,
6239,
2051,
1773,
18319,
23440,
21682,
20718,
2136,
16736,
18058};
#endif


#if (P==30803)
unsigned int inverse_factors_table[15] = {
15402,
7701,
9626,
4452,
13975,
9605,
1040,
3495,
17037,
2700,
20492,
15568,
4620,
28724,
9821};
#endif


#if (P==35507)
unsigned int inverse_factors_table[16] = {
17754,
8877,
11096,
18447,
28228,
7397,
34829,
33600,
14935,
34758,
28396,
4353,
23378,
7140,
27055,
31727};
#endif


#if (P==35117)
unsigned int inverse_factors_table[16] = {
17559,
26338,
24143,
12483,
11160,
20718,
433,
11904,
8121,
915,
29534,
21110,
32487,
33968,
20872,
13999};
#endif


#if (P==36629)
unsigned int inverse_factors_table[16] = {
18315,
27472,
6868,
27901,
26293,
22732,
18521,
33485,
31535,
15504,
14518,
9058,
35033,
19815,
7974,
33361};
#endif


#if (P==40787)
unsigned int inverse_factors_table[16] = {
20394,
10197,
12746,
5895,
501,
6279,
25599,
24859,
6044,
25571,
19644,
929,
6514,
13716,
19012,
1750};
#endif


#if (P==42677)
unsigned int inverse_factors_table[16] = {
21339,
32008,
8002,
16504,
17402,
36289,
7332,
27881,
31283,
42479,
39204,
26815,
22129,
16743,
25513,
3565};
#endif


#if (P==48371)
unsigned int inverse_factors_table[16] = {
24186,
12093,
15116,
37223,
12805,
38706,
7824,
25661,
12498,
10045,
119,
14161,
36126,
38296,
23267,
33428};
#endif


#if (P==52667)
unsigned int inverse_factors_table[16] = {
26334,
13167,
42792,
29008,
3405,
7285,
35556,
10468,
31664,
39884,
32055,
42522,
9707,
4586,
17263,
21283};
#endif


#if (P==58171)
unsigned int inverse_factors_table[16] = {
29086,
14543,
47264,
2954,
466,
42643,
58160,
121,
14641,
56917,
1899,
57770,
44459,
10272,
49961,
42082};
#endif


#if (P==61717)
unsigned int inverse_factors_table[16] = {
30859,
46288,
11572,
47011,
10068,
25310,
35357,
39614,
52554,
25449,
55120,
9924,
47161,
2675,
58170,
52658};
#endif


#if (P==83579)
unsigned int inverse_factors_table[17] = {
41790,
20895,
67908,
25139,
28502,
59703,
54596,
45339,
82995,
6740,
44203,
78926,
3448,
20486,
26037,
16100,
31521};
#endif

/*********** extra *******/


#if (P==10883)
unsigned int inverse_factors_table[14] = {
5442,
2721,
3401,
9055,
503,
2700,
9273,
1946,
10515,
4828,
9081,
4070,
974,
1855};
#endif


#if (P==13331)
unsigned int inverse_factors_table[14] = {
6666,
3333,
4166,
11925,
3848,
9694,
3417,
11264,
6569,
12645,
4011,
10935,
8486,
11465};
#endif


#if (P==14341)
unsigned int inverse_factors_table[14] = {
7171,
10756,
2689,
2857,
2420,
5272,
1126,
5868,
683,
7577,
3906,
12353,
8369,
13058};
#endif


#if (P==13829)
unsigned int inverse_factors_table[14] = {
6915,
10372,
2593,
2755,
11733,
9423,
10749,
13535,
3462,
9530,
5857,
8529,
3301,
13178};
#endif


#if (P==15373)
unsigned int inverse_factors_table[14] = {
7687,
11530,
10569,
3543,
8481,
12467,
5059,
12809,
9825,
3558,
7385,
10194,
11529,
2883};
#endif


#if (P==16421)
unsigned int inverse_factors_table[15] = {
8211,
12316,
3079,
5324,
2330,
9970,
4587,
5268,
334,
13030,
4181,
8817,
2475,
592,
5623};
#endif


#if (P==20611)
unsigned int inverse_factors_table[15] = {
10306,
5153,
6441,
17149,
10453,
6298,
9240,
6838,
12496,
1080,
12184,
9434,
2058,
10109,
2543};
#endif


#if (P==21011)
unsigned int inverse_factors_table[15] = {
10506,
5253,
6566,
18795,
15093,
18398,
20205,
19306,
7507,
3547,
16631,
1357,
13492,
15771,
17234};
#endif


#if (P==22531)
unsigned int inverse_factors_table[15] = {
11266,
5633,
7041,
7481,
20888,
18260,
13862,
10676,
15178,
14740,
1167,
10029,
2457,
21072,
10767};
#endif


#if (P==23563)
unsigned int inverse_factors_table[15] = {
11782,
5891,
19145,
8560,
16233,
5260,
4638,
21588,
12730,
10149,
8328,
9675,
13389,
21580,
20831};
#endif


#if (P==24709)
unsigned int inverse_factors_table[15] = {
12355,
18532,
4633,
17277,
10009,
9795,
21687,
14863,
10309,
2072,
18527,
17010,
22419,
5792,
17151};
#endif


#if (P==25603)
unsigned int inverse_factors_table[15] = {
12802,
6401,
8001,
8501,
15335,
24273,
2293,
9234,
8766,
8153,
6021,
24196,
8218,
20413,
1744};
#endif


#if (P==28933)
unsigned int inverse_factors_table[15] = {
14467,
21700,
5425,
5764,
8612,
11265,
87,
7569,
2421,
16775,
27200,
23190,
27362,
8736,
21375};
#endif


#if (P==30851)
unsigned int inverse_factors_table[15] = {
15426,
7713,
9641,
25669,
12754,
18044,
15333,
16269,
9632,
6467,
18984,
21725,
17027,
11882,
7748};
#endif


#if (P==32771)
unsigned int inverse_factors_table[16] = {
16386,
8193,
10241,
10881,
27309,
11834,
13073,
2564,
19896,
9907,
32275,
16619,
29944,
28576,
32769,
4};
#endif


#if (P==35339)
unsigned int inverse_factors_table[16] = {
17670,
8835,
28713,
12838,
28487,
19712,
10639,
32843,
10352,
16056,
32470,
32513,
35001,
8227,
9344,
23006};
#endif


#if (P==36877)
unsigned int inverse_factors_table[16] = {
18439,
27658,
25353,
8499,
27835,
1455,
15036,
25286,
8370,
27477,
2708,
31618,
36208,
5037,
36870,
49};
#endif


#if (P==40973)
unsigned int inverse_factors_table[16] = {
20487,
30730,
28169,
9443,
13001,
12376,
8302,
6618,
38760,
21582,
1660,
10409,
14669,
30338,
17745,
7520};
#endif


#if (P==43013)
unsigned int inverse_factors_table[16] = {
21507,
32260,
8065,
8569,
4570,
23595,
6766,
12924,
10297,
1164,
21493,
32442,
41280,
35392,
12091,
34107};
#endif


#if (P==49157)
unsigned int inverse_factors_table[16] = {
24579,
36868,
9217,
9793,
46699,
44610,
29269,
15322,
39009,
47146,
13247,
41676,
24695,
1283,
23908,
44025};
#endif


#if (P==53267)
unsigned int inverse_factors_table[16] = {
26634,
13317,
16646,
47649,
27860,
26143,
40839,
34151,
9836,
14024,
10812,
31546,
16022,
10811,
9923,
28513};
#endif


#if (P==57347)
unsigned int inverse_factors_table[16] = {
28674,
14337,
17921,
19041,
11947,
51473,
38329,
54142,
6912,
5693,
9194,
158,
24964,
11447,
53261,
7419};
#endif


#if (P==58379)
unsigned int inverse_factors_table[16] = {
29190,
14595,
47433,
21208,
27448,
11709,
26789,
55853,
17365,
15690,
50236,
48284,
37670,
10547,
27214,
5802};
#endif


#if (P==63493)
unsigned int inverse_factors_table[16] = {
31747,
47620,
11905,
12649,
58334,
11714,
9423,
29715,
47567,
46434,
21062,
45746,
30729,
3545,
58904,
42738};
#endif


#if (P==65539)
unsigned int inverse_factors_table[17] = {
32770,
16385,
20481,
21761,
21846,
58257,
6473,
20308,
43476,
17816,
4479,
6507,
2855,
24189,
41068,
65537,
4};
#endif


#if (P==86027)
unsigned int inverse_factors_table[17] = {
43014,
21507,
69897,
31252,
22973,
69111,
25254,
46365,
70549,
69316,
13879,
12188,
64742,
33043,
71192,
20159,
79760};
#endif
