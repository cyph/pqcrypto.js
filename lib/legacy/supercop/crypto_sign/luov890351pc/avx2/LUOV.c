#include "LUOV.h"

#define PK_SEED(pk) (pk)
#define PK_Q2(pk)   (pk + 32)

#define BIG_SK_SEED(sk) (sk)
#define _BIG_SK_T(sk) (BIG_SK_SEED(sk) + PRIVATE_SEED_BYTES )
#define _BIG_SK_P1(sk) (_BIG_SK_T(sk) + COL_BYTES*(VINEGAR_VARS+1))
#define _BIG_SK_L(sk) (_BIG_SK_P1(sk) + COL_BYTES*((VINEGAR_VARS+1)*(VINEGAR_VARS+2)/2 ))
#define BIG_SK_T(sk) ((bitcontainer *) _BIG_SK_T(sk))
#define BIG_SK_P1(sk)((bitcontainer *) _BIG_SK_P1(sk))
#define BIG_SK_L(sk) ((bitcontainer *) _BIG_SK_L(sk))


#define BIG_PK_Q1(big_pk) ((bitcontainer *) big_pk)
#define BIG_PK_Q2(big_pk) ((bitcontainer *) (big_pk+Q1_BYTES))

/*
	Calculates Q_2, the last OIL_VARS*(OIL_VARS+1)/2 columns of the macaulay matrix of the public system

	T  : the secret linear map
	Q1 : the part of the public map that is generated from the public seed
	Q2 : receives the remaining part of the public map
*/
#if OIL_VARS == 63
void calculateQ2(const bitcontainer *T , const bitcontainer *Q1, bitcontainer *Q2) {
	int i, j, k, col;

	// Allocate memory for temporary matrices that will store the values P_i,1 T + P_i,2 for i from 1 to OIL_VARS.
	// These OIL_VARS matrices are bitsliced into one OIL_VARS by OIL_VARS array of bitcontainers.
	alignas(32) bitcontainer TempMat[VINEGAR_VARS+1][64] = {{0}};

	col = 0;
	// Simultaneously calculate P_i,1*T + P_i,2 for all i from 1 to OIL_VARS
	for (i = 0; i <= VINEGAR_VARS; i++) {
		// Calculates P_i,1*T
		for (j = i; j <= VINEGAR_VARS; j++) {
			__m256i rrrr = _mm256_permute4x64_epi64(_mm256_loadu_si256((__m256i *)&Q1[col++]),0);
			__m256i TJ = {T[j]<<3,T[j]<<2,T[j]<<1,T[j]};

			for (k = 7; k >= 0; k--)
			{
				*((__m256i *)&TempMat[i][k*8+4]) ^=  (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TJ);
				TJ = _mm256_slli_epi64(TJ,4);

				*((__m256i *)&TempMat[i][k*8]) ^= (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TJ);
				TJ = _mm256_slli_epi64(TJ,4);
			}
		}
		// Add P_i,2
		for (j = 0; j < OIL_VARS; j++) {
			TempMat[i][j] ^= Q1[col++];
		}
	}

	// Calculate P_i,3 = Transpose(T)*TempMat_i, and store the result in Q_2
	alignas (32) bitcontainer temp[OIL_VARS][64] = {{0}};
	for (i = 0; i < OIL_VARS; i++) {
		for (j = 0; j <= VINEGAR_VARS; j++) {
			__m256i rrrr = _mm256_permute4x64_epi64(_mm256_loadu_si256((__m256i *)&TempMat[j][i]),0);
			__m256i TJ = {T[j]<<3,T[j]<<2,T[j]<<1,T[j]};

			for (k = 7; k >=0 ; k--)
			{
				*((__m256i *)&temp[i][k*8+4]) ^= (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TJ);
				TJ = _mm256_slli_epi64(TJ,4);

				*((__m256i *)&temp[i][k*8+0]) ^= (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TJ);
				TJ = _mm256_slli_epi64(TJ,4);
			}
		}
	}

	// Convert from P_1,3 to Q2
	col = 0;
	for (i = 0; i < OIL_VARS; i++) {
		for (j = i; j < OIL_VARS; j++) {
			Q2[col] = temp[i][j];
			if (j != i){
				Q2[col] ^= temp[j][i];
			}
			col ++ ;
		}
	}
}
#else
void calculateQ2(const bitcontainer *T , const bitcontainer *Q1, bitcontainer *Q2) {
	int i, j, k, col;

	// Allocate memory for temporary matrices that will store the values P_i,1 T + P_i,2 for i from 1 to OIL_VARS.
	// These OIL_VARS matrices are bitsliced into one OIL_VARS by OIL_VARS array of bitcontainers.
#if OIL_VARS <=64
	alignas(32) bitcontainer TempMat[VINEGAR_VARS+1][(OIL_VARS+7)/8*8] = {{0}};
#else
	alignas(32) bitcontainer TempMat[VINEGAR_VARS+1][(OIL_VARS+7)/8*8] = {{{0}}};
#endif

	alignas (32) unsigned char table[32*4]= {REPEAT8(  1),REPEAT8(  1),REPEAT8(  2),REPEAT8(  2),
											 REPEAT8(  4),REPEAT8(  4),REPEAT8(  8),REPEAT8(  8),
											 REPEAT8( 16),REPEAT8( 16),REPEAT8( 32),REPEAT8( 32),
											 REPEAT8( 64),REPEAT8( 64),REPEAT8(128),REPEAT8(128)};
	__m256i *masks = (__m256i *) table;

	col = 0;
	// Simultaneously calculate P_i,1*T + P_i,2 for all i from 1 to OIL_VARS
	for (i = 0; i <= VINEGAR_VARS; i++) {
		// Calculates P_i,1*T
		for (j = i; j <= VINEGAR_VARS; j++) {
			bitcontainer r = _mm_loadu_si128(&Q1[col++]);
			__m256i rr = _mm256_permute2x128_si256(_mm256_loadu_si256((__m256i *)&r),_mm256_setzero_si256(),0);
			
			uint8_t *t = (uint8_t *)&T[j];
			for (k=0; k<OIL_VARS; k+=8)
			{
				__m256i tttt = _mm256_set1_epi8(t[k/8]);

				__m256i t1t2 = _mm256_cmpeq_epi8(tttt & masks[0],_mm256_setzero_si256());
				*((__m256i *)&TempMat[i][k+0]) ^= _mm256_andnot_si256(t1t2,rr);

				__m256i t3t4 = _mm256_cmpeq_epi8(tttt & masks[1],_mm256_setzero_si256());
				*((__m256i *)&TempMat[i][k+2]) ^= _mm256_andnot_si256(t3t4,rr);

				__m256i t5t6 = _mm256_cmpeq_epi8(tttt & masks[2],_mm256_setzero_si256());
				*((__m256i *)&TempMat[i][k+4]) ^= _mm256_andnot_si256(t5t6,rr);

				__m256i t7t8 = _mm256_cmpeq_epi8(tttt & masks[3],_mm256_setzero_si256());
				*((__m256i *)&TempMat[i][k+6]) ^= _mm256_andnot_si256(t7t8,rr);
			}
		}
		// Add P_i,2
		for (j = 0; j < OIL_VARS; j++) {
			bitcontainer r = _mm_loadu_si128(&Q1[col++]);
			TempMat[i][j] ^= r;
		}
	}
	
	// Calculate P_i,3 = Transpose(T)*TempMat_i, and store the result in Q_2
	alignas (32) bitcontainer temp[OIL_VARS][(OIL_VARS+7)/8*8] = {{{0}}};
	for (i = 0; i < OIL_VARS; i++) {
		for (j = 0; j <= VINEGAR_VARS; j++) {
			bitcontainer r = TempMat[j][i];
			__m256i rr = _mm256_permute2x128_si256(_mm256_loadu_si256((__m256i *)&r),_mm256_setzero_si256(),0);
			
			uint8_t *t = (uint8_t *) &T[j];
			for (k = 0; k < OIL_VARS; k+=8)
			{
				__m256i tttt = _mm256_set1_epi8(t[k/8]);

				__m256i t1t2 = _mm256_cmpeq_epi8(tttt & masks[0],_mm256_setzero_si256());
				*((__m256i *)&temp[i][k+0]) ^= _mm256_andnot_si256(t1t2,rr);

				__m256i t3t4 = _mm256_cmpeq_epi8(tttt & masks[1],_mm256_setzero_si256());
				*((__m256i *)&temp[i][k+2]) ^= _mm256_andnot_si256(t3t4,rr);

				__m256i t5t6 = _mm256_cmpeq_epi8(tttt & masks[2],_mm256_setzero_si256());
				*((__m256i *)&temp[i][k+4]) ^= _mm256_andnot_si256(t5t6,rr);

				__m256i t7t8 = _mm256_cmpeq_epi8(tttt & masks[3],_mm256_setzero_si256());
				*((__m256i *)&temp[i][k+6]) ^= _mm256_andnot_si256(t7t8,rr);
			}
		}
	}
	
	// Move from P_i,3 to Q2
	col = 0;
	for (i = 0; i < OIL_VARS; i++) {
		for (j = i; j < OIL_VARS; j++) {
			bitcontainer result = temp[i][j];
			if (j != i) result ^= temp[j][i];
			_mm_storeu_si128(&Q2[col],result);
			col ++ ;
		}
	}
}
#endif

/*
	Calculates a convenient representation of the central map F

	T : the secret linear map
	Q1 : the part of the public map that is generated from the public seed
	P1 : receives the vinegar x vinegar part of the central map (i.e. the part that becomes constant after substituting the vinegar variables)
	L  : receives the vinegar x oil part of the central map (i.e. the part that becomes linear after substituting the vinegar variables)
*/
#if OIL_VARS == 63
void TransformQ1(bitcontainer *T , const bitcontainer *Q1 , bitcontainer *P1, bitcontainer *L){
	int i,j,k;
	int colQ1 = 0;
	int colP1 = 0;

	// erase the bits of T that are not used
	for (i = 0; i < VINEGAR_VARS+1; i++)
	{
		T[i] &= (uint64_t) 0x7fffffffffffffff;
	}

	// initilize L to all zeros
	memset(L,0,COL_BYTES*(VINEGAR_VARS+1)*OIL_VARS);

	// go through the columns of Q1
	for (i = 0; i <= VINEGAR_VARS; i++)
	{
		// go through vinegar x vinegar columns
		for (j=i; j<= VINEGAR_VARS ; j++)
		{
			bitcontainer r = Q1[colQ1];
			// copy column to P1
			P1[colP1++] = r;

			// update L
			__m256i rrrr = _mm256_permute4x64_epi64(_mm256_loadu_si256((__m256i *)&r),0);
			__m256i TI = {T[i]<<3,T[i]<<2,T[i]<<1,T[i]};
			__m256i TJ = {T[j]<<3,T[j]<<2,T[j]<<1,T[j]};
			for(k=7;k>=0; k--){
				_mm256_storeu_si256((__m256i *)&L[j*OIL_VARS+k*8+4],_mm256_loadu_si256((__m256i *)&L[j*OIL_VARS+k*8+4]) ^ (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TI) );
				TI = _mm256_slli_epi64(TI,4);
				_mm256_storeu_si256((__m256i *)&L[j*OIL_VARS+k*8],_mm256_loadu_si256((__m256i *)&L[j*OIL_VARS+k*8]) ^ (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TI));
				TI = _mm256_slli_epi64(TI,4);

				_mm256_storeu_si256((__m256i *)&L[i*OIL_VARS+k*8+4],_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+k*8+4]) ^ (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TJ) );
				TJ = _mm256_slli_epi64(TJ,4);
				_mm256_storeu_si256((__m256i *)&L[i*OIL_VARS+k*8],_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+k*8]) ^ (__m256i) _mm256_blendv_pd(_mm256_setzero_pd(),(__m256d) rrrr,(__m256d)TJ));
				TJ = _mm256_slli_epi64(TJ,4);
			}
			colQ1++;
		}

		// go through vinegar x oil columns
		for(j=0;j<OIL_VARS;j++)
		{
			bitcontainer result = _mm_loadu_si128(&L[i*OIL_VARS+j]);
			result ^= _mm_loadu_si128(&Q1[colQ1++]);
			_mm_storeu_si128(&L[i*OIL_VARS+j],result);
		}
	}
}
#else
void TransformQ1(bitcontainer *T , const bitcontainer *Q1 , bitcontainer *P1, bitcontainer *L){
	int i,j,k;
	int colQ1 = 0;
	int colP1 = 0;

	// erase the bits of T that are not used
#if OIL_VARS == 90
	bitcontainer mask = _mm_set_epi64x(0x3ffffff,0xffffffffffffffff);
	for (i = 0; i < VINEGAR_VARS+1; i++)
	{
		bitcontainer result = _mm_loadu_si128(&T[i]);
		result &= mask;
		_mm_storeu_si128(&T[i],result);
	}
#elif OIL_VARS == 117
	bitcontainer mask = _mm_set_epi64x(0x1fffffffffffff,0xffffffffffffffff);
	for (i = 0; i < VINEGAR_VARS+1; i++)
	{
		bitcontainer result = _mm_loadu_si128(&T[i]);
		result &= mask;
		_mm_storeu_si128(&T[i],result);
	}
#else
	Error: parameters not supported
#endif

	// prepare masks
	alignas (32) unsigned char table[32*4]= {REPEAT8(1 ),REPEAT8(1 ),REPEAT8(2  ),REPEAT8(2  ),
											 REPEAT8(4 ),REPEAT8(4 ),REPEAT8(8  ),REPEAT8(8  ),
											 REPEAT8(16),REPEAT8(16),REPEAT8(32 ),REPEAT8(32 ),
											 REPEAT8(64),REPEAT8(64),REPEAT8(128),REPEAT8(128)};
	__m256i *masks = (__m256i *) table;
	
	// initialize L to all zeros
	memset(L,0,COL_BYTES*(VINEGAR_VARS+1)*OIL_VARS);

	// go through all columns
	for (i = 0; i <= VINEGAR_VARS; i++)
	{
		// go through vinegar x vinegar columns
		for (j=i; j<= VINEGAR_VARS ; j++)
		{
			// copy column to P1
			bitcontainer r = _mm_loadu_si128(&Q1[colQ1]);
			_mm_storeu_si128(&P1[colP1++],r); 
			
			// update L
			__m256i rr = _mm256_permute2x128_si256(_mm256_loadu_si256((__m256i *)&r),_mm256_setzero_si256(),0);
			uint8_t *ti = (uint8_t *) &T[i];
			uint8_t *tj = (uint8_t *) &T[j];

			for(k=0;k<(OIL_VARS+7)/8; k++){

				__m256i titi = _mm256_set1_epi8(ti[k]);

				__m256i ti1ti2 = _mm256_cmpeq_epi8(titi & masks[0],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[j*OIL_VARS+k*8+0],_mm256_loadu_si256((__m256i *)&L[j*OIL_VARS+k*8+0]) ^ _mm256_andnot_si256(ti1ti2,rr));

				__m256i ti3ti4 = _mm256_cmpeq_epi8(titi & masks[1],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[j*OIL_VARS+k*8+2],_mm256_loadu_si256((__m256i *)&L[j*OIL_VARS+k*8+2]) ^ _mm256_andnot_si256(ti3ti4,rr));

				__m256i ti5ti6 = _mm256_cmpeq_epi8(titi & masks[2],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[j*OIL_VARS+k*8+4],_mm256_loadu_si256((__m256i *)&L[j*OIL_VARS+k*8+4]) ^ _mm256_andnot_si256(ti5ti6,rr));

				__m256i ti7ti8 = _mm256_cmpeq_epi8(titi & masks[3],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[j*OIL_VARS+k*8+6],_mm256_loadu_si256((__m256i *)&L[j*OIL_VARS+k*8+6]) ^ _mm256_andnot_si256(ti7ti8,rr));


				__m256i tjtj = _mm256_set1_epi8(tj[k]);

				__m256i tj1tj2 = _mm256_cmpeq_epi8(tjtj & masks[0],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[i*OIL_VARS+k*8+0],_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+k*8+0]) ^ _mm256_andnot_si256(tj1tj2,rr));

				__m256i tj3tj4 = _mm256_cmpeq_epi8(tjtj & masks[1],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[i*OIL_VARS+k*8+2],_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+k*8+2]) ^ _mm256_andnot_si256(tj3tj4,rr));

				__m256i tj5tj6 = _mm256_cmpeq_epi8(tjtj & masks[2],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[i*OIL_VARS+k*8+4],_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+k*8+4]) ^ _mm256_andnot_si256(tj5tj6,rr));

				__m256i tj7tj8 = _mm256_cmpeq_epi8(tjtj & masks[3],_mm256_setzero_si256());
				_mm256_storeu_si256((__m256i *)&L[i*OIL_VARS+k*8+6],_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+k*8+6]) ^ _mm256_andnot_si256(tj7tj8,rr));
			}
			colQ1++;
		}

		// go through vinegar x oil columns
		for(j=0;j<OIL_VARS;j++)
		{
			bitcontainer result = _mm_loadu_si128(&L[i*OIL_VARS+j]);
			result ^= _mm_loadu_si128(&Q1[colQ1++]);
			_mm_storeu_si128(&L[i*OIL_VARS+j],result);
		}
	}
}
#endif

/*
	Does a precomputation to make signing faster

	sk : A secret key
	big_sk : receives a bigger secret key that can be used to sign faster
*/
void precompute_sign(unsigned char *big_sk, const unsigned char *sk){
	memcpy(BIG_SK_SEED(big_sk),sk,PRIVATE_SEED_BYTES );

	// Calculate public seed
	Sponge sponge;
	unsigned char publicseed[PUBLIC_SEED_BYTES];
	initializeAndAbsorb(&sponge , sk, PRIVATE_SEED_BYTES);
	squeezeBytes(&sponge, publicseed , PUBLIC_SEED_BYTES);

	// Calculate T
	BIG_SK_T(big_sk)[0]=empty; /* makes T linear instead of affine*/
	squeezeCols(&sponge , &(BIG_SK_T(big_sk)[1]) , VINEGAR_VARS);

	// Calculate Q1
	bitcontainer *Q1 = aligned_alloc(32,Q1_BYTES);
	calculateQ1(publicseed,Q1);

	// Calculate P1 and L
	TransformQ1(BIG_SK_T(big_sk),Q1,BIG_SK_P1(big_sk),BIG_SK_L(big_sk));

	free(Q1);
}

/*
	Writes Q2 (the columns of the public map that are not generated from the public seed) to the char array "data".  

	data : receives the serialization of Q2
	Q2 : the part of the publlic key that is not generated from the public seed
*/
#if OIL_VARS <= 64
void serializeQ2(unsigned char *data, bitcontainer *Q2){
	int bitsWritten = 0;
	int i;
	for (i = 0; i < Q2_COLS ; i++) {
		bitcontainer a = Q2[i];
		int bits = OIL_VARS;
		
		// fill up the current byte
		if(bitsWritten > 0){
			*data ^= (uint64_t) (a&((((uint64_t)1)<<(9-bitsWritten))-1)) << bitsWritten;
			data ++;
			bits -= (8-bitsWritten);
			a >>= (8-bitsWritten);
			bitsWritten = 0;
		}

		// write full bytes as as long as possible
		while(bits >=8){
			*data = a&255;
			data ++;
			a>>=8;
			bits -=8;
		}

		// write the remainder of the column in the next byte
		while (bits > 0)
		{
			if (bitsWritten == 0){
				*data = 0;
			}
			*data ^= ( (-(a&1))^ *data) & (1 << bitsWritten);
			bitsWritten++;
			if (bitsWritten == 8) {
				bitsWritten = 0;
				data++;
			}

			a >>= 1;
			bits--;
		}
	}
}
#else
void serializeQ2(unsigned char *data, bitcontainer *Q2){
	int bitsWritten = 0;
	int i;
	for (i = 0; i < Q2_COLS ; i++) {

		alignas (32) uint64_t a[2];
		_mm_store_si128((__m128i*) a, _mm_loadu_si128(&Q2[i]));

		int bits = 64;
		// fill up the current byte
		if(bitsWritten > 0){
			*data ^= (uint64_t) (a[0]&((((uint64_t)1)<<(9-bitsWritten))-1)) << bitsWritten;
			data ++;
			bits -= (8-bitsWritten);
			a[0] >>= (8-bitsWritten);
			bitsWritten = 0;
		}

		// write full bytes from a[0] as long as possible
		while(bits >=8){
			*data = a[0]&255;
			data ++;
			a[0]>>=8;
			bits -=8;
		}

		// write the remainder of a[0] to the next byte
		while (bits > 0)
		{
			if (bitsWritten == 0){
				*data = 0;
			}
			*data ^= ( (-(a[0]&1))^ *data) & (1 << bitsWritten);
			bitsWritten++;
			if (bitsWritten == 8) {
				bitsWritten = 0;
				data++;
			}

			a[0] >>= 1;
			bits--;
		}

		bits = OIL_VARS-64;
		// fill up the current byte
		if(bitsWritten > 0){
			*data ^= (uint64_t) (a[1]&((((uint64_t)1)<<(9-bitsWritten))-1)) << bitsWritten;
			data ++;
			bits -= (8-bitsWritten);
			a[1] >>= (8-bitsWritten);
			bitsWritten = 0;
		}

		// write full bytes from a[1] as long as possible
		while(bits >=8){
			*data = a[1]&255;
			data ++;
			a[1]>>=8;
			bits -=8;
		}

		// write the remainder of a[1] to the next byte
		while (bits > 0)
		{
			if (bitsWritten == 0){
				*data = 0;
			}
			*data ^= ( (-(a[1]&1))^ *data) & (1 << bitsWritten);
			bitsWritten++;
			if (bitsWritten == 8) {
				bitsWritten = 0;
				data++;
			}

			a[1] >>= 1;
			bits--;
		}
	}
}
#endif

/*
	Evaluates the vinegar x vinegar part of the secret map.

	vinegar_variables : an assignment to the vinegar variables
	P1 : The coefficients of the vinegar x vinegar part of the secret map
	evaluation : receives the evaluation of the vinegar x vinegar part of the secret map at the given vinegar variables
*/
#if OIL_VARS == 63
void EvaluateVinegarPart(FELT *vinegar_variables, const bitcontainer *P1, FELT *evaluation){
	int i,j;

	// copy the vinegar variables in __m256i variables
	__m256i vin[VIN_BLOCKS] = {{0}};
	memcpy(vin,vinegar_variables,VINEGAR_VARS+1);

	// prepare an array to store scalar multiples of the vinegar variables
	__m256i products[VIN_BLOCKS];
	FELT *productsFELT = (FELT *)products;

	__m256i O[8] = {{0}};
	int col = 0;
	for (i=0 ; i <= VINEGAR_VARS; i++) {
		// prepare the product of all the vinegar variables by the i-th vinegar variable
		for(j= i/32 ; j<VIN_BLOCKS ; j++){
			products[j] = _mm256_setzero_si256();
			scalarMul_ct(&products[j], vin[j],vinegar_variables[i]);
		}

		// add the x_i*x_i , ... , x_i*x_v terms to O in groups of 4
		for (j = i; j <= VINEGAR_VARS; j+=4)
		{
			addScalarProduct4AVX(O, &productsFELT[j], _mm256_loadu_si256((__m256i *)&P1[col]));
			col += 4;
		}
		col -= (j-VINEGAR_VARS-1);
	}

	// convert the contents of O to the evaluation by xoring and permuting stuff
	for (i = 0; i < 8; i++)
	{
		bitcontainer *out = (bitcontainer *) &O[i];
		bitcontainer xor = out[0]^out[1]^out[2]^out[3];
		FELT *bytes = (FELT *) &xor;
		for(j=0;j<8;j++){
			if((i!=7) || (j!=7)){
				evaluation[i+j*8] = bytes[j];
			}
		}
	}
}
#elif (OIL_VARS == 90) || (OIL_VARS == 117)
void EvaluateVinegarPart(FELT *vinegar_variables, const bitcontainer *P1, FELT* evaluation){
	int i,j,col;

	// copy the vinegar variables in __m256i variables
	__m256i vin[VIN_BLOCKS] = {{0}};
	memcpy(vin,vinegar_variables,VINEGAR_VARS+1);

    // prepare an array to store scalar multiples of the vinegar variables
	__m256i products[VIN_BLOCKS+1] = {{0}};
	FELT *productsFELT = (FELT *)products;

	__m256i O[8] = {{0}};
	col = 0;
	for (i=0 ; i <= VINEGAR_VARS; i++) {
		// prepare the product of all the vinegar variables by the i-th vinegar variable
		for(j= i/32 ; j<VIN_BLOCKS ; j++){
			products[j] = _mm256_setzero_si256();
			scalarMul_ct(&products[j], vin[j],vinegar_variables[i]);
		}

		// add the x_i*x_i , ... , x_i*x_v terms to O in groups of 2
		for (j = i; j <= VINEGAR_VARS; j+=2)
		{	
			addScalarProduct4AVX(O, &productsFELT[j],_mm256_loadu_si256((__m256i*)&P1[col]));
			col += 2;
		}

		col -= (j-VINEGAR_VARS-1);
	}

	// convert the contents of O to the evaluation by xoring and permuting stuff
	for (i = 0; i < 8; i++)
	{
		alignas (32) bitcontainer out[2];
		_mm256_store_si256((__m256i*) out,O[i]);
		bitcontainer xor = out[0]^out[1];
		FELT *bytes = (FELT *) &xor;
		for(j=0;j<16;j++){
			if(i+j*8 < OIL_VARS){
				evaluation[i+j*8] = bytes[j];
			}
		}
	}

	printEvaluation(evaluation);
}
#endif

/*
	Takes a vector U of 256 bits, and a vector of 32 Field elements a, computes the entrywise product of U 
	with aaaaaaaa (repeated 8 times) and xor the answer to O[0],...,0[7] (in some permuted way)

	O : the array where the products are xored to
	a : an array of 32 Field elements
	U : a vector of 256 bits
*/
static inline 
void myAVXOp(__m256i *O, __m256i a, __m256i U) {
	static const char maskList[32*8] = {REPEAT32(1), REPEAT32(2), REPEAT32(4), REPEAT32(8), REPEAT32(16), REPEAT32(32), REPEAT32(64), REPEAT32(128)};
	__m256i *masks = (__m256i *)&maskList;

	int i;
	for (i=0 ; i<8 ; i++){
		__m256i Temp = _mm256_and_si256(U,masks[i]);
		Temp = _mm256_cmpeq_epi8(Temp,_mm256_setzero_si256());
		Temp = _mm256_andnot_si256(Temp,a);
		O[i] = _mm256_xor_si256(O[i],Temp);
	}
	
}


/*
	From an assgnment to the vinegar variables, P1 and L, this calculates the augmented matrix of the linear system of equations F(v|.) = 0

	A : receives the augmented matrix
	vinegar_variables : an assignment to the vinegar variables
	P1 : the vinegar x vinegar part of the private map
	L  : the vinegar x oil part of the private map
*/
#if OIL_VARS == 63
void BuildAugmentedMatrixFromP1L(Matrix *A, FELT *vinegar_variables, const bitcontainer *P1, const bitcontainer *L) {
	int i,j,k,l;

	// multiplies the binary matrices L[i*OIL_VARS : (i+1)*OIL_VARS-1] by the i-th vinegar variable and xor the result to O
	__m256i O[(OIL_VARS+3)/4][8] = {{{0}}};
	for(i=0; i<= VINEGAR_VARS ; i++){
		__m256i a = _mm256_set1_epi8(vinegar_variables[i]);
		for(j=0 ; j< OIL_VARS ; j+=4){
			myAVXOp(O[j/4],a,_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+j]));
		}
	}

	// convert from O to the linear part of the augmented matrix
	for(i=0; i<16; i++){
		for(j=0; j < 8; j++){
			FELT *bytes = (FELT *) &O[i][j];
			for (k=0; k<3 ; k++){
				for (l=0; l<7; l++){
					A->array[j+8*l][i*4+k] = bytes[k*8+l];	
				}
				if(j!=7){
					A->array[j+8*7][i*4+k] = bytes[k*8+7];	
				}
			}
			if(i!=15){
				for (l=0; l<7; l++){
					A->array[j+8*l][i*4+3] = bytes[3*8+l];	
				}
				if(j!=7){
					A->array[j+8*7][i*4+3] = bytes[3*8+7];	
				}
			}
		}
	}

	// compute the constant part of F(v|.)
	FELT evaluation[OIL_VARS];
	EvaluateVinegarPart(vinegar_variables,P1,evaluation);

	// move it to the augmented matrix
	for(i=0 ; i<OIL_VARS ; i++){
		A->array[i][OIL_VARS] = evaluation[i];
	}
}

#elif (OIL_VARS == 90) || (OIL_VARS == 117)

void BuildAugmentedMatrixFromP1L(Matrix *A, FELT *vinegar_variables, const bitcontainer *P1, const bitcontainer *L) {
	int i,j,k;

	// multiplies the binary matrices L[i*OIL_VARS : (i+1)*OIL_VARS-1] by the i-th vinegar variable and xor the result to O
	__m256i O[(OIL_VARS+1)/2][8] = {{{0}}};
	for(i=0; i<= VINEGAR_VARS ; i++){
		__m256i a = _mm256_set1_epi8(vinegar_variables[i]);
		for(j=0 ; j< OIL_VARS ; j+=2){
			myAVXOp(O[j/2],a,_mm256_loadu_si256((__m256i *)&L[i*OIL_VARS+j]));
		}
	}

	// convert from O to the linear part of the augmented matrix
	for(i=0; i<(OIL_VARS+1)/2 ; i++){
		for(j=0; j < 8; j++){
			FELT *bytes = (FELT *) &O[i][j];
			for (k = 0; k < 16; k++)
			{
				if(j+8*k < OIL_VARS){
					A->array[j+8*k][i*2] = bytes[k];
				}
			}
			for (k = 16; k < 32; k++)
			{
				if(j+8*(k-16) < OIL_VARS){
					A->array[j+8*(k-16)][i*2+1] = bytes[k];
				}
			}
		}
	}

	// compute the constant part of F(v|.)
	FELT evaluation[OIL_VARS];
	EvaluateVinegarPart(vinegar_variables,P1,evaluation);

	// move it to the augmented matrix
	for(i=0 ; i<OIL_VARS ; i++){
		A->array[i][OIL_VARS] = evaluation[i];
	}
}
#endif

/*
	Builds the augmented matrix for the system F(x) = target , after fixing the vinegar variables

	A                 : Receives the augmented matrix
	vinegar_variables : An assignment to the vinegar variables
	T                 : The matrix that determines the linear transformation T
	Q1                : The first part of the public map P
*/
void BuildAugmentedMatrixFromQ1(Matrix *A, FELT *vinegar_variables, const bitcontainer *T, const bitcontainer *Q1) {
	int i, j, k ;

	// copy the vinegar variables in __m256i variables
	__m256i Vin[VIN_BLOCKS];
	memcpy(Vin,vinegar_variables,VINEGAR_VARS+1);

	// allocate memory for temp = (P1 + P1^t)T
	FELT **temp = malloc(sizeof(FELT*)*(VINEGAR_VARS+1));
	for( k = 0 ; k<= VINEGAR_VARS ; k++){
		temp[k] = malloc(OIL_BLOCKS*32*sizeof(FELT));
	}
	__m256i permuted_temp[VINEGAR_VARS+1][OIL_BLOCKS] = {{{0}}};

	// allocate memory for the constant part
	FELT constant_part[OIL_BLOCKS*32]= {0};
	__m256i permuted_constant_part[OIL_BLOCKS] = {{0}};

	// allocate memory for the permuted augmented matrix
	__m256i permuted_A[OIL_VARS][OIL_BLOCKS] = {{{0}}};

	// allocate memory for the product of the vinegar variables by a scalar
	__m256i products[VIN_BLOCKS];
	FELT *productsFELT = (FELT *)products;

	// calculate constant part and temp = (P1+P1^t)T
	int col = 0;
	for (i=0; i<=VINEGAR_VARS; i++) {
		// calculate the product of the vinegar variables by the  i-th vinegar variable
		for(j=i/32; j<VIN_BLOCKS ; j++){
			products[j] = _mm256_setzero_si256();
			scalarMul_ct(&products[j], Vin[j],vinegar_variables[i]);
		}
		
		// update permuted_constant_part and permuted_temp
		for (j=i; j <= VINEGAR_VARS; j++) {
			addScalarProduct3AVX(permuted_constant_part, productsFELT[j], permuted_temp[j], vinegar_variables[i], permuted_temp[i],vinegar_variables[j],Q1[col++]);
		}

		// update permuted_A
		for (j=0; j < OIL_VARS; j++) {
			addScalarProductAVX(permuted_A[j],vinegar_variables[i],Q1[col++]);
		}
	}	

	// unscramble permuted_constant_part, permuted_temp and permuted_A
	permute(permuted_constant_part,constant_part);
	for(i=0 ; i<VINEGAR_VARS+1 ; i++){
		permute(permuted_temp[i],temp[i]);
	}
	for(i=0 ; i<OIL_VARS ; i++){
		permute(permuted_A[i],(FELT *)A->array[i]);
	}

	// transpose A
	for(i = 0 ; i< OIL_VARS ; i++){
		for(j = i+1 ; j<OIL_VARS ; j++){
			FELT swap = A->array[i][j];
			A->array[i][j] = A->array[j][i];
			A->array[j][i] = swap;
		}
	}

	// compute T*temp = T*(P1+P1^t)*T ans add it to A
	__m256i buff1[OIL_BLOCKS];
	FELT buff2[OIL_BLOCKS*32];
	for (k = 0; k < OIL_VARS; k++)	{
		for (i=0 ;i<OIL_BLOCKS ;i++){
			buff1[i] = _mm256_setzero_si256();
		}

		for (i = 0; i <= VINEGAR_VARS; i++) {
			addScalarProductAVX(buff1,temp[i][k],T[i]);
		}
		permute(buff1,buff2);
		for (i=0 ; i<OIL_VARS ; i++){
			A->array[k][i] ^= buff2[i];
		}
	}

	// move constant part to last column of A
	for(i=0 ; i<OIL_VARS ; i++){
		A->array[i][OIL_VARS] = constant_part[i];
	}

	// free up memory
	for(k=0 ; k<=VINEGAR_VARS ; k++){
		free(temp[k]);
	}
	free(temp);
}

/*
	Solves the system F(x) = target for x

	T : The secret linear transformation (only used if fast =0)
	Q1 : The first part of the public map (only used if fast =0)
	big_sk : The secret key (only used if fast=1)
	vinegar_sponge : The sponge object used to generate the assignment to the vinegar variables
	signature : A signature object, used to store the solution x 
	fast : 0 of 1 in the case that we are using T & Q1 or a big public key respectively
*/
void solvePrivateUOVSystem(const bitcontainer *T, const bitcontainer *Q1, const unsigned char *big_sk, FELT *target, Sponge *vinegar_sponge , unsigned char *signature, const int fast) {
	int i;
	Matrix A = newMatrix(OIL_VARS, OIL_BLOCKS*32 );
	A.cols = OIL_VARS+1;
	int solution_found = 0;
	
    // Repeatedly try an assignment to the vinegar variables until a unique solution is found
	while (solution_found == 0) {		
		// Set homogenizing variable to one and squeeze an assignment to the vinegar variables from the vinegar sponge
		signature[0] = 1;
		squeezeVector(vinegar_sponge , &(signature[1]) , VINEGAR_VARS);
		
		// Print vinegar values if KAT is defined
		printVinegarValues(&(signature[1]));

		// Build the augmented matrix
		if(fast){
			BuildAugmentedMatrixFromP1L(&A, signature, BIG_SK_P1(big_sk), BIG_SK_L(big_sk));
		} else {
			BuildAugmentedMatrixFromQ1(&A, signature, T , Q1);
		}

		// Add the target to the last column
		for(i=0 ; i<OIL_VARS ; i++){
			A.array[i][OIL_VARS] ^= target[i];
		}

		// Print augmented matrix if KAT is defined
		printAugmentedMatrix(A);
		
		// Try to find a unique solution to the linear system
		solution_found = getUniqueSolution(A,&(signature[1+VINEGAR_VARS]));

		// Report whether a solution is found if KAT is defined
		reportSolutionFound(solution_found);
	}
	// Free the memory occupied by the augmented matrix
	destroy_matrix(A);
}

#ifndef MESSAGE_RECOVERY
/*
	Computes the target vector by hashing the document, after padding it with a 0x00 byte
	(Only used in appended signature mode)

	document : The document that is being signed
	len : The number of bytes of the document being signed
	target : receives the target vector
 */
void computeTarget(const unsigned char* document , uint64_t len, FELT *target){
	Sponge sponge;
	unsigned char pad = 0;

	// prepare sponge
	Keccak_HashInitialize_SHAKE(&sponge);
	Keccak_HashUpdate(&sponge,document , len*8);
	Keccak_HashUpdate(&sponge,&pad , 8);
	Keccak_HashFinal(&sponge , 0);

	//squeeze target
	squeezeVector(&sponge  , target , OIL_VARS);
}
#else
/*
	Computes the target vector. 
	The document is hashed, after padding it with a 0x01 byte, to get the first part of the target.
	Then, the first part is hashed again and xored with the last part of the padded document to get the second part of the target.
	(Only used in message recovery mode)

	document : The document that is being signed
	len : The number of bytes of the document being signed
	target : receives the target vector
 */
void computeTarget(const unsigned char* document , uint64_t len, FELT *target){
	int i,start_recovery;
	Sponge sponge;
	unsigned char buf[FIRST_PART_TARGET + SECOND_PART_TARGET];
	unsigned char pad = 1;

	// Compute first part of the target and put in the first part of the buffer 
	Keccak_HashInitialize_SHAKE(&sponge);
	Keccak_HashUpdate(&sponge,document , len*8);
	Keccak_HashUpdate(&sponge,&pad , 8);
	Keccak_HashFinal(&sponge , 0);
	squeezeBytes(&sponge  , buf , FIRST_PART_TARGET );

	// Absorb first part of target into a Sponge object and squeeze into the second part of the buffer
	initializeAndAbsorb(&sponge, buf , FIRST_PART_TARGET);
	squeezeBytes(&sponge  , &(buf[FIRST_PART_TARGET]) , SECOND_PART_TARGET );

	// If not the entire document can be covered from a signature, we xor the last part of the message 
	// into the second part of the buffer and we xor the last byte with a 0x01.
	// Otherwise, we xor the entire document into the second part of the buffer, and we xor the next byte with a 0x01
	if(len > RECOVERED_PART_MESSAGE){
		start_recovery = len- RECOVERED_PART_MESSAGE;
		buf[FIRST_PART_TARGET + SECOND_PART_TARGET - 1] ^= 1;
	}
	else{
		start_recovery = 0;
		buf[FIRST_PART_TARGET + len] ^= 1;
	}
	for(i = start_recovery ; i<len ; i++){
		buf[FIRST_PART_TARGET + i-start_recovery] ^= document[i];
	}

	// Interpret the contents of the buffer as a list of fied elements.
	memcpy(target,buf,OIL_VARS);
}
#endif

/*
	If message recovery is enabled, this function extracts the last part of the document from the evaluated signature and appends it to the first part of document

	document : Initially this contains the first part of the document, after the call to this function this contains the entire original document
	len      : pointer to the length of document, which is altered appropriately
	evaluation : The evaluation of the public map in the signature
*/
void extractMessage(unsigned char *document ,unsigned long long *len , FELT *evaluation){
	#ifdef MESSAGE_RECOVERY
	int i, reading;
	unsigned char buf[FIRST_PART_TARGET+SECOND_PART_TARGET];
	unsigned char buf2[SECOND_PART_TARGET];
	Sponge sponge;

	// Interpret the evaluation of P as an array of bytes
	memcpy(buf,evaluation,OIL_VARS);

	// Absorb the first part of the buffer into a Sponge object and squeeze into buffer 2
	initializeAndAbsorb(&sponge, buf , FIRST_PART_TARGET );
	squeezeBytes(&sponge  , buf2 , SECOND_PART_TARGET );

	// Xor the secon part of the evaluation into buffer 2
	for(i = 0 ; i<SECOND_PART_TARGET ; i++ ){
		buf2[i] ^= buf[FIRST_PART_TARGET + i];
	}

	// Start searching from the left for the first byte equal to 0x01
	// All bytes before this byte are appended to the document and len is increased
	reading = 0;
	unsigned long long oldlen = *len; 
	for (i = SECOND_PART_TARGET-1; i >= 0 ; i--)
	{
		if(reading){
			document[oldlen + i] = buf2[i];
		}
		else{
			if(buf2[i] == 1){
				reading = 1;
				*len += i;
			}
		}
	}

	#endif
}

/*
	Same as myAVXOp2, but xors all the results in a single __m256i value, rather than in 8 different ones
*/
static inline 
void myAVXOp2(__m256i *O, __m256i *a, __m256i U) {
	static const unsigned char maskList[32*8] = {REPEAT32(1), REPEAT32(2), REPEAT32(4), REPEAT32(8), REPEAT32(16), REPEAT32(32), REPEAT32(64), REPEAT32(128)};
	__m256i *masks = (__m256i *)&maskList;

	int i;
	for (i=0 ; i<8 ; i++){
		__m256i Temp = _mm256_and_si256(U,masks[i]);
		Temp = _mm256_cmpeq_epi8(Temp,_mm256_setzero_si256());
		Temp = _mm256_andnot_si256(Temp,a[i]);
		*O = _mm256_xor_si256(*O,Temp);
	}
}

/*
	Takes a solution x to the secret system F(x) = H(d), and converts it to a solution x' of P(x') = H(d)
	by applying the inverse of T (which is equal to T itself).

	signature: the signaure to transform
	T : the secret linear transformation
*/
#if OIL_VARS == 63
void multiplyByT(FELT *signature , bitcontainer * T){
	int i,j;

	// copy oil variables into Oil variables, while repeating each set of 8 consecutive oil variables 4 times
	__m256i Oil[8];
	for(i=0 ; i<8 ; i++){
		FELT *a = (FELT*)&Oil[i];
		for(j=0 ; j<8 ; j++){
			a[j+0 ] = signature[VINEGAR_VARS+1+i+j*8];
			a[j+8 ] = signature[VINEGAR_VARS+1+i+j*8];
			a[j+16] = signature[VINEGAR_VARS+1+i+j*8];
			a[j+24] = signature[VINEGAR_VARS+1+i+j*8];
		}
	}

	// since there are only 63 oil variables, set the memory locations that would contain the 64-th one to 0
	Oil[7] = _mm256_insert_epi8(Oil[7],0,7);
	Oil[7] = _mm256_insert_epi8(Oil[7],0,7+8);
	Oil[7] = _mm256_insert_epi8(Oil[7],0,7+16);
	Oil[7] = _mm256_insert_epi8(Oil[7],0,7+24);

	// For each i, compute inner product of the i-th row of T with the oil variables, and add the result to the i-th vinegar variable
	// we do this for 4 values of i simultaneously for efficiency
	for (i = 1; i <= VINEGAR_VARS; i+=4) {
		__m256i O = {0};
		myAVXOp2(&O,Oil,_mm256_loadu_si256((__m256i*) &T[i]));
		FELT *bytes = (FELT *)&O;
		for (j = 0; j < 8; j++)
		{
			signature[i+0] ^= bytes[j];
			signature[i+1] ^= bytes[j+8];
			signature[i+2] ^= bytes[j+16];
			signature[i+3] ^= bytes[j+24];
		}
	}
}
#elif OIL_VARS > 64
void multiplyByT(FELT *signature , bitcontainer * T){
	int i,j;

	// copy oil variables into Oil variables, while repeating each set of 16 consecutive oil variables 2 times
	__m256i Oil[8] = {{0}};
	for(i=0 ; i<8 ; i++){
		FELT *a = (FELT*)&Oil[i];
		for(j=0 ; j<16 ; j++){
			if(i+j*8< OIL_VARS){
				a[j+0 ] = signature[VINEGAR_VARS+1+i+j*8];
				a[j+16] = signature[VINEGAR_VARS+1+i+j*8];
			}
		}
	}

	// For each i, compute inner product of the i-th row of T with the oil variables, and add the result to the i-th vinegar variable
	// we do this for 2 values of i simultaneously for efficiency
	for (i = 1; i <= VINEGAR_VARS-1; i+=2) {
		__m256i O = {0};
		myAVXOp2(&O,Oil,_mm256_loadu_si256((__m256i*) &T[i]));
		FELT *bytes = (FELT *)&O;
		for (j = 0; j < 16; j++)
		{
			signature[i+0] ^= bytes[j];
			signature[i+1] ^= bytes[j+16];
		}
	}

	// if the number of vinegar variables is odd, we need to do the last value of i separately
	if(i!= VINEGAR_VARS+1){
		__m256i O = {0};
		myAVXOp2(&O,Oil,_mm256_loadu_si256((__m256i*) &T[i]));
		FELT *bytes = (FELT *)&O;
		for (j = 0; j < 16; j++)
		{
			signature[i+0] ^= bytes[j];
		}
	}
}
#endif

/*
	Initializes the sponge object that will later be used to squeeze the vinegar variables from

	vinegar_sponge : the sponge object
	document : the document to sign
	doclen : the length of the document to sign
	sk : the secret key
*/
void initializeVinegarSponge(Sponge *vinegar_sponge, const unsigned char *document, uint64_t doclen, const unsigned char *sk){
	// Define the appropriate padding, based on wheter we are in message recovery mode or not
	unsigned char pad = 0;
	#ifdef MESSAGE_RECOVERY
		pad = 1;
	#endif

	// Initialize the vinegar sponge from the padded document and the private key
	Keccak_HashInitialize_SHAKE(vinegar_sponge);
	Keccak_HashUpdate(vinegar_sponge, document, doclen*8 );
	Keccak_HashUpdate(vinegar_sponge, &pad , 8);
	Keccak_HashUpdate(vinegar_sponge, sk , PRIVATE_SEED_BYTES*8);
	Keccak_HashFinal(vinegar_sponge, 0 );
}

/* 
	Evaluated the public map P in a signature

	big_pk : The public key (after precomputation)
	signature : The point that P is evaluated in
	evaluation : Receives the vector P(signature)
*/
void evaluatePublicMap(const unsigned char *big_pk, unsigned char *signature , FELT* evaluation){
	int i,j,col;
	bitcontainer *Q1 = (bitcontainer *) big_pk;
	
	// copy the signature into __m256i variables
	__m256i sig[VAR_BLOCKS] = {{0}};
	memcpy(sig,signature,VARS+1);
	
	// initialize __m256i variables to receive the evaluation
	__m256i eval[OIL_BLOCKS] = {{0}};

	// to contain the scalar multiple of the signature by a scalar
	__m256i products[VAR_BLOCKS+1] = {{0}};
	FELT *productsFELT = (FELT *)products;

#if OIL_VARS <=64
	alignas(32) bitcontainer Out[4][256] = {{0}};
#else
	alignas(32) bitcontainer Out[4][256] = {{{0}}};
#endif

	col = 0;
	// Accumulate the columns of the public key in Out, according to the values of the products x0^2,x0*x1,...,x_n^2
	for (i=0 ; i <= VARS; i++) {
		// calculate the values of xi^2, ... ,xi*xn
		for(j= i/32 ; j<VAR_BLOCKS ; j++){
			products[j] = _mm256_setzero_si256();
			scalarMul(&products[j], sig[j],signature[i]);
		}
		
		// xor the columns of the public map in the right column of Out
		accumulateCols(&productsFELT[i], &Q1[col], VARS+1-i, Out);
		col += (VARS+1-i);
	}

	// matrix multiplication of Out by vector (0,1,...,255)^t of all field elements
	for(i=0 ; i<256 ; i++){
		Out[0][i] ^= Out[1][i] ^ Out[2][i] ^ Out[3][i];
		addScalarProductAVX(eval,i,Out[0][i]);
	}

	// unshuffle the contents of eval
	permute(eval,evaluation);
	
	// print evaluation if KAT is defined
	printEvaluation(evaluation);
}

static inline
uint64_t read_uint64_t(const unsigned char *data){
	return (((uint64_t) data[0])<< 0) | (((uint64_t) data[1])<< 8) |
		   (((uint64_t) data[2])<<16) | (((uint64_t) data[3])<<24) |
		   (((uint64_t) data[4])<<32) | (((uint64_t) data[5])<<40) |
		   (((uint64_t) data[6])<<48) | (((uint64_t) data[7])<<56);
}

/*
	Reads Q2, the part of the public map that is not generated from the public key from "data", and store it in Q2

	data : the serialization of Q2
	Q2 : receives the columns of Q2 that are read from "data"
*/
#if OIL_VARS == 63
void deserializeQ2(const unsigned char *data, bitcontainer *Q2){
	int bitsUsed = 0;
	int i;
	for (i = 0; i < Q2_COLS ; i++) {
		Q2[i] = empty;
		int written = 0;

		//read the remainder of the current byte
		if(bitsUsed>0){
			Q2[i] |= ((*data) >> bitsUsed);
			data ++;
			written = 8-bitsUsed;
			bitsUsed = 0;
		}

		// read the rest of the column
		bitcontainer in = *((bitcontainer *) data );
		Q2[i] |= ((in &((((bitcontainer)1)<<(OIL_VARS-written))-1) ) << written);
		bitsUsed = (OIL_VARS-written)%8;
		data += (OIL_VARS-written)/8;
	}
}
#elif (OIL_VARS == 90) || (OIL_VARS == 117)
void deserializeQ2(const unsigned char *data, bitcontainer *Q2){
	int bitsUsed = 0;
	int i;
	for (i = 0; i < Q2_COLS ; i++) {
		uint64_t q2_0 = 0;
		uint64_t q2_1 = 0;
		int written = 0;

		// read the remainder of the current byte
		if(bitsUsed>0){
			q2_0 = (((uint64_t)(*data)) >> bitsUsed);
			data ++;
			written = 8-bitsUsed;
			bitsUsed = 0;
		}

		// read the rest of q2_0
		uint64_t in = read_uint64_t(data);
		q2_0 |= ((in &((~((uint64_t)0))>>(written)) ) << written);
		bitsUsed = (64-written)%8;
		data += (64-written)/8;

		// read the remainder of the current byte
		written = 0;
		if(bitsUsed>0){
			q2_1 = (((uint64_t)(*data)) >> bitsUsed);
			data ++;
			written = 8-bitsUsed;
			bitsUsed = 0;
		}

		// read the rest of q2_1
		in = read_uint64_t(data);
		q2_1 |= ((in &((((uint64_t)1)<<(OIL_VARS-64-written))-1) ) << written);
		bitsUsed = (OIL_VARS-64-written)%8;
		data += (OIL_VARS-64-written)/8;

		// put q2_0 and q2_1 together in Q2
		Q2[i] = _mm_set_epi64x(q2_1,q2_0);
	}
}
#endif

/*
	Generates a key pair (pk, sk) and also stores the intermediate results T,Q1, and Q2.

	pk : receives the public key
	sk : receives the secret key
	T : receives the secret linear transformation
	Q1 : receives the part of the public map that is generated from the public seed
	Q2 : receives the rest of the public map
*/
void _generateKeyPair(unsigned char *pk, unsigned char *sk, bitcontainer *T, bitcontainer *Q1, bitcontainer *Q2) {
	randombytes(sk , 32);
	// Calculate public seed
	Sponge sponge;
	initializeAndAbsorb(&sponge , sk, 32);
	squeezeBytes(&sponge, PK_SEED(pk) , 32);
	
	// Calculate T
	_mm_storeu_si128(&T[0],empty); /* makes T linear instead of affine*/
	squeezeCols(&sponge , &(T[1]) , VINEGAR_VARS);
	
	// Calculate Q1
	calculateQ1(PK_SEED(pk),Q1);
	
	// Calculates Q_2, the part of the public map P that cannot be generated from the public seed
	calculateQ2(T,Q1,Q2);
	
	// Write Q2 to the public key
	serializeQ2(PK_Q2(pk),Q2);
}

/*
	Produces a signature sig for a document of length doclen, given a secret key/precomputed secret key if fast is equal to 0 or 1 respectively.

	sig : receives the signature
	sk : a secret key or a big secret key
	document : the document to sign
	doclen : the length of the document to sign
	fast : 0 or 1 if we use a compact secret key or a precomputed secret key respectively
*/
void _sign(unsigned char *sig, const unsigned char *sk, const unsigned char *document , uint64_t doclen, const int fast) {
	FELT target[OIL_VARS];
	
	// to temporary hold a signature + a homogenizing variable
	unsigned char signature[VARS+1];
	
	// compute the target for the public map P
	computeTarget(document, doclen , target);
	
	// prepare a vinegar sponge
	Sponge vinegar_sponge;
	initializeVinegarSponge(&vinegar_sponge, document, doclen, sk);
	
	bitcontainer *T = NULL;
	bitcontainer *Q1 = NULL;
	if(fast == 0){
		// if slow approach, we calculate T and Q1
		Sponge sponge;
		initializeAndAbsorb(&sponge , sk , 32);
		unsigned char publicseed[32];
		squeezeBytes(&sponge , publicseed , 32);
		Q1 = aligned_alloc(32,Q1_BYTES);
		
		calculateQ1(publicseed, Q1);
		
		T = aligned_alloc(32,sizeof(bitcontainer[VINEGAR_VARS+1]));
		T[0] = empty; // this makes the linear map T linear, picking the first row of T random would make T an affine transformation
		squeezeCols(&sponge, &(T[1]) , VINEGAR_VARS);
	}
	else{
		T = BIG_SK_T(sk);
	}

	// Generate a solution to F(x) = target
	solvePrivateUOVSystem(T, Q1, sk, target, &vinegar_sponge , signature, fast);
	
	// Print solution to the equation F(x) = target if KAT is defined
	printPrivateSolution(signature);
	
	// Convert into a solution for P(x) = target
	multiplyByT(signature,T);
	
	// copy the signature to the output, except for the homogenizing variable
	memcpy(sig,&signature[1],VARS);

	// free memory
	if(fast == 0){
		free(Q1);
		free(T);
	}
}

/*
	Generates a key pair

	pk : receives the public key
	sk : receives the secret key
*/
void generateKeyPair(unsigned char *pk, unsigned char *sk) {
	// allocate memory for the intermediate results
	bitcontainer *T = aligned_alloc(32,T_BYTES);
    bitcontainer *Q1 = aligned_alloc(32,Q1_BYTES);
    bitcontainer *Q2 = aligned_alloc(32,Q2_BYTES);

    // generate the key pair
	_generateKeyPair(pk,sk,T,Q1,Q2);

	// free the memory of the intermediate results
	free(T);
	free(Q1);
	free(Q2);
}

/*
	Verifies a signature for a document

	pk : the public key
	sig : a signature
	document : a char array containing a document
	len : the length of the document

	returns : 0 if the signature is valid, -1 otherwise
*/
int verify(const unsigned char *pk, const unsigned char *sig, unsigned char *document , unsigned long long *doclen) {
    int valid;

    // allocate memory for a precomputed public key and compute it
    unsigned char *big_pk = aligned_alloc(32,BIG_PUBLIC_KEY_BYTES);
    precompute_verify(big_pk,pk);

    // Do the verification using the precomputed public key
    valid = verify_fast(big_pk,sig,document,doclen);

    // free memory
    free(big_pk);
    return valid;
}

/*
	Generates a compact public key and a precomputed public key, as well as a precomputed secret key

	pk : receives the small public key
	big_pk : receives the precomputed public key
	big_sk : receives the precomputed secret key
*/
void generateBigKeyPair(unsigned char *pk, unsigned char *big_pk, unsigned char *big_sk){
	// generate key pair
	_generateKeyPair(pk,BIG_SK_SEED(big_sk),BIG_SK_T(big_sk),BIG_PK_Q1(big_pk),BIG_PK_Q2(big_pk));

	// compute P1 and L
	TransformQ1(BIG_SK_T(big_sk),BIG_PK_Q1(big_pk),BIG_SK_P1(big_sk),BIG_SK_L(big_sk));
}

/*
	Produces a big public key from a small public key.

	big_pk : receives the precomputed public key
	pk : the compact public key
*/
void precompute_verify(unsigned char *big_pk , const unsigned char *pk){
	// calculate Q1
	calculateQ1(PK_SEED(pk),BIG_PK_Q1(big_pk));
	
	// deserialize Q2
	deserializeQ2(PK_Q2(pk),BIG_PK_Q2(big_pk));
}

/*
	Verifies a document with a big public key

	big_pk : a precomputed public key
	sig : a signature
	document : a docoment to check a signature for, if we are in message recovery mode, a part of the document is appended
	doclen : the length of the document, if we are in message recovery mode, this length is increased
*/
int verify_fast(const unsigned char *big_pk , const unsigned char *sig, unsigned char* document, unsigned long long *doclen){
	int i;
	FELT evaluation[OIL_BLOCKS*32];
	FELT target[OIL_VARS];

	unsigned char signature[VARS+1];
	signature[0]=1;
	memcpy(&signature[1],sig,VARS);
	
	// Evaluate the public map P at the signature
	evaluatePublicMap(big_pk, signature , evaluation);
	
	// If we are in message recovery mode, we extracts a part of the document from the signature
	extractMessage(document , doclen , evaluation);
	
	// We compute the target based on the full document
	computeTarget(document, *doclen, target);

	// Output 0 if the evaluation of the public map is equal to the target, otherwise output -1
	for(i=0 ; i<OIL_VARS ; i++){
		if ( target[i] != evaluation[i]){
			return -1;
		}
	}
	return 0;
}
