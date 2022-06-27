#ifndef PARAMS_H
#define PARAMS_H

#define GFBITS 13
#define SYS_T 128

#define COND_BYTES ((1 << (GFBITS-4))*(2*GFBITS - 1))
#define IRR_BYTES ((SYS_T * GFBITS)/8)

#define PK_NROWS (SYS_T*GFBITS) 
#define PK_NCOLS ((1 << GFBITS) - PK_NROWS)

#define SK_BYTES (IRR_BYTES + COND_BYTES)
#define SYND_BYTES (PK_NROWS / 8)

#define GFMASK ((1 << GFBITS) - 1)

#endif

