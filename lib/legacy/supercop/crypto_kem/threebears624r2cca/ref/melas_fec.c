#include "namespace.h"
/* Melas forward error correction, reference code (as implemented in the paper) */
#include "melas_fec.h"
typedef uint32_t fec_gf_t;
static const fec_gf_t Q = 0x211;

/* Return s/2^n mod R */
static fec_gf_t step(unsigned n, fec_gf_t R, fec_gf_t s) {
    for (; n; n--) s = (s^((s&1)*R))>>1;
    return s;
}

/* Compute syndrome(data), where data has length len */
#define syndrome18(data,len) s18update(0,data,len)
static fec_gf_t s18update(fec_gf_t r, const uint8_t *data, unsigned len) {
    for (unsigned i=0; i<len; i++) {
        r = step(8,0x46231,r^data[i]);
    }
    return r;
}

/* Append 3 bytes of FEC(data) to data, so that the FEC becomes 0 */
void melas_fec_set(
    uint8_t out[MELAS_FEC_BYTES],
    const uint8_t *data,
    unsigned len
) {
    fec_gf_t fec = syndrome18(data,len);
    for (unsigned i=0; i<MELAS_FEC_BYTES; i++, fec>>=8) {
        out[i] = fec;
    }
}

/* Return a*b mod Q */
static fec_gf_t mul(fec_gf_t a, fec_gf_t b) {
    fec_gf_t r = 0;
    for (unsigned i=0; i<9; i++) {
        r ^= ((b>>(8-i))&1) * a;
        a = step(1,Q,a);
    }
    return r;
}

/* Reverse an 18-bit number x */
static fec_gf_t reverse18(fec_gf_t x) {
    fec_gf_t ret = 0;
    for (unsigned i=0; i<18; i++) {
        ret ^= ((x>>i)&1)<<(17-i);
    }
    return ret;
}

/* Correct data to have the given FEC */
void melas_fec_correct (
    uint8_t *data,
    unsigned len,
    const uint8_t fec[MELAS_FEC_BYTES]
) {
    
    fec_gf_t a=s18update(syndrome18(data,len),fec,MELAS_FEC_BYTES);
    fec_gf_t c,r,htr;
    unsigned i;
    
    /* Form a quadratic equation from the syndrome */
    c = mul(step(9,Q,a),step(9,Q,reverse18(a)));
    for (i=0,r=0x100; i<510; i++) r = mul(r,c);
    r = step(17,Q,r);
    a = step(511-(len+MELAS_FEC_BYTES)*8,Q,a);
    
    /* Solve using the half trace */
    const uint8_t table[9] = {36,10,43,215,52,11,116,244,0};
    for (i=0,htr=0; i<9; i++) htr ^= ((r>>i)&1)*table[i];
    fec_gf_t e0 = mul(a,htr), e1 = e0^a;

    /* Correct the errors using the locators */
    for (i=0; i<len; i++) {
        data[i] ^= e0 & (((e0 & (e0-1))-1)>>9);
        data[i] ^= e1 & (((e1 & (e1-1))-1)>>9);
        e0 = step(8,Q,e0);
        e1 = step(8,Q,e1);
    }
}
