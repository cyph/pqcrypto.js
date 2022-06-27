#ifndef __PHOTON_H_
#define __PHOTON_H_

#include <stdint.h>

#define ROUND_N			   20
#define STEP_N             4
#define STEP_INTER_ROUND_N 5

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))

#define D				8

typedef uint8_t	byte;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef uint32_t CWord;
typedef u32 tword;

typedef struct{
	u64 h;
	u64 l;
}u128; // state word


#ifdef _TABLE_
void BuildTableSCShRMCS();
#endif

void PrintState(byte state[D][D]);

extern byte PHOTON_key_state[D/2][D];

void TEM_PHOTON_Permutation(
	unsigned char *state_inout,
	const unsigned char *tweak_in, unsigned char domain);

#endif /*  end of photon.h */
