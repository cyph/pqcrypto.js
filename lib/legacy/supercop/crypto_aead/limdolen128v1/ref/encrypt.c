//
//  encrypt.c
//  Limdolen 128
//

#include "crypto_aead.h"
#include "api.h"
#define ROUNDS 16
#define CRYPTO_BYTES 16

// generalized pentagonal numbers
unsigned char roundConsts[CRYPTO_BYTES] = { 
        0x00, 0x01, 0x02, 0x05, 0x07, 0x0c, 0x0f, 0x16,
        0x1a, 0x23, 0x28, 0x33, 0x39, 0x46, 0x4d, 0x5c 
    };

/*
// calculates the generalized penagonal number of roundNum
// if another 16 bytes of memory isn't an issue,
// these can be stored in an array, as shown above
unsigned char getRoundConst(unsigned char roundNum) {
    // the generalized penagonal numbers are in a series of penagonal_number(n)
    // where n is in the series 0, 1, -1, 2, -2, 3 ... etc
    return ( ((-5 + pow(-1, roundNum) - (6 * roundNum)) * (-1 + pow(-1, roundNum) - (6 * roundNum))) / 96 );
}
*/

// c (ciphertxt input) and k (key) will always be 128bits
// c is the input to the round, the output is written back to this
//   input buffer as it is processed
void e128Rounds(unsigned char *c, const unsigned char *k) {
    for(int round_num = 0; round_num < ROUNDS; round_num++) {
    
        // less memory, more processing 
        // unsigned char roundConst = getRoundConst(round_num);
        //  .vs.
        // more memory, less processing
        unsigned char roundConst = roundConsts[round_num];
        // pointers to positions in the input array
        unsigned char q=0,r=4,s=8,t=12;
        unsigned char z = 0;
        // this is a temp placeholder variable to enable
        //   the permutation of the 'q' parameter
        unsigned char rotater = 0;

        while(q<4) {
            // round functions
            c[r] = c[r] ^ k[r] ^ roundConst;
            c[s] = c[s] ^ k[s] ^ roundConst;

            z = c[r] & c[s];
            c[q] = c[q] ^ k[q] ^ roundConst ^ ((z << 2) | (z >> 6));
            c[t] = c[t] ^ k[t] ^ roundConst ^ ((z << 7) | (z >> 1));
            z = c[q] & c[t];
            c[r] = c[r] ^ ((z << 3) | (z >> 5));
            z = c[s] ^ ((z << 5) | (z >> 3));

            // permute
            c[s]=c[q];
            if(q==0) {
                // save this to put in the first position after round
                rotater = z;
            } else {
                // rotate this byte back one position
                c[q-1] = z;
            }

            z=c[t];
            c[t] = c[r];
            c[r] = z;
            
            // go to next byte
            q+=1;
            r+=1;
            s+=1;
            t+=1;
        }
        // put saved byte into the first postion
        c[3] = rotater;
    }
}


// nonce must be mutable, it's used as the input block for the function
static void blockEncryptor(const unsigned char *key, const unsigned char *in, unsigned char *out, unsigned char *nonce,  unsigned long long len) {
    // get total number of blocks
    unsigned long long numBlocks = len / CRYPTO_BYTES + ( len % CRYPTO_BYTES == 0 ? 0 : 1);
    // generate ciphertext
    for(unsigned long long x = 0; x < numBlocks; x++) {
        // take nonce and use round function as PRNG to get a block to xor with plaintext
        e128Rounds(nonce, key);
        // set output to the output of the PRNG XOR plaintext
        // xor one block at a time, until the end of the plaintext
        for (int i = 0; i<CRYPTO_BYTES && x*CRYPTO_BYTES+i < len; i++) {
            out[x*CRYPTO_BYTES+i] = nonce[i] ^ in[x*CRYPTO_BYTES+i];
        }
        // increment the counter-nonce
        for(int i = CRYPTO_BYTES-1; i >= 0; i--) {
            nonce[i]++;
            if(nonce[i]) break;
        }
    }
}

static void makeTag(const unsigned char *adKey, const unsigned char *ad, unsigned long long adlen,
                    const unsigned char *m, unsigned long long mlen, unsigned char *tag) {
    unsigned char alpha[CRYPTO_BYTES]= {0};
    unsigned char invAlphaX[CRYPTO_BYTES];
    unsigned char alphaX[CRYPTO_BYTES];
    
    //gets the alpha value for use in the AAD functions
    e128Rounds(alpha, adKey);
    // calculate 'alpha * x' and 'alpha / x'
    for(int i=0; i<CRYPTO_BYTES; i++) {
        alphaX[i] = alpha[i]<<1;
        invAlphaX[i] = alpha[i]>>1;
    }

    //get blocks for both ad and pt
    unsigned long long numADBlocks = (adlen+mlen) / CRYPTO_BYTES;

    // if there's not enough data to make a full block,
    // of if there's additional data beyond a smooth block boundry,
    // add an extra block to contain it.
    if(numADBlocks==0 || ((adlen+mlen) % CRYPTO_BYTES) != 0) {
        numADBlocks++;
    }

    unsigned char tempEncIn[CRYPTO_BYTES];
    
    // these pointers keep track of the position in the input as output is written
    unsigned long long aadPointer=0;
    unsigned long long mPointer=0;
    // create blocks of the concantination of the ad and pt arrays,
    // both arrays may be sizes other than multiples of the block size
    // one or both array could be zero; 
    // do all but the final block, final block has special steps (see below)
    for(unsigned long long i=0; i< numADBlocks-1; i++) {
        for(int j=0;j<CRYPTO_BYTES;j++) {
            if(adlen > aadPointer) {
                tempEncIn[j] = ad[aadPointer];
                aadPointer++;
            } else if(mlen > mPointer) {
                tempEncIn[j] = m[mPointer];
                mPointer++;
            } 
        }

        for(int j=0;j<CRYPTO_BYTES;j++) {
            // take the input block and xor with either alpha or alpha * X
            if (i%2 == 0) {
                tempEncIn[j] = tempEncIn[j] ^ alpha[j];
            } else {
                tempEncIn[j] = tempEncIn[j] ^ alphaX[j];
            }
        }
        e128Rounds(tempEncIn, adKey);
        for(int j=0;j<CRYPTO_BYTES;j++) {
            tag[j] ^= tempEncIn[j];
        }
    }
    // "padding" marker byte either goes at the last byte in the last block
    //    or at the end of both the AAD and the message
    char paddingStart = CRYPTO_BYTES-1;
    // now do the final block:
    // (any leftover aad + message + padding) XOR
    //    current aggregate tag value XOR inverse alpha
    for(int j=0;j<CRYPTO_BYTES;j++) {
        if(adlen > aadPointer) {
            tag[j] = ad[aadPointer] ^ tag[j] ^ invAlphaX[j];
            aadPointer++;
        } else if(mlen > mPointer) {
            tag[j] = m[mPointer] ^ tag[j] ^ invAlphaX[j];
            mPointer++;
        } else {
            // any bytes in the block after mlen+adlen
            tag[j] = tag[j] ^ invAlphaX[j];
            // set padding block marker to be the first byte after
            //   the aad and message are done with processing
            paddingStart = j;
        }
    }
    if(adlen==0)
        tag[paddingStart] ^= 0xC0;
    else
        tag[paddingStart] ^= 0x80;
    // create final tag value
    e128Rounds(tag, adKey);
}

int crypto_aead_encrypt(
                        unsigned char *c, unsigned long long *clen,
                        const unsigned char *m,unsigned long long mlen,
                        const unsigned char *ad,unsigned long long adlen,
                        const unsigned char *nsec,
                        const unsigned char *npub,
                        const unsigned char *k
                        )
{
//        ... the code for the cipher implementation goes here,
//        ... generating a ciphertext c[0],c[1],...,c[*clen-1]
//        ... from a plaintext m[0],m[1],...,m[mlen-1]
//        ... and associated data ad[0],ad[1],...,ad[adlen-1]
//        ... and nonce npub[0],npub[1],...
//        ... and secret key k[0],k[1],...
//        ... the implementation shall not use nsec
    
    // ciphertext is always same length as the plaintext + tag
    *clen = mlen+CRYPTO_BYTES;
    
    // calculate the tag from associated data + plaintext
    unsigned char adKey[CRYPTO_BYTES];

    // prepare round input from nonce
    for(int i =0; i<CRYPTO_BYTES;i++) {
        // set adKey to the nonce to prep for encryption
        adKey[i] = npub[i];
        // clear out the first <CRYPTO_BYTES> of 'c' to use as the tag
        // since this is passed by reference to the makeTag function,
        //   and xor'd with, it needs to start at 0x00;
        c[i]= 0x00;
    }
    
    // create AAD key by encrypting nonce with key
    e128Rounds(adKey, k);

    // calculate the tag
    makeTag(adKey, ad, adlen, m, mlen, c);
    
    // // encrypt // //
    // value to pass into the encryption function
    unsigned char tempEncIn[CRYPTO_BYTES];

    // set up the nonce: calculated tag + provided nonce (which could be 0x00's)
    for(int j=0;j<CRYPTO_BYTES;j++) {
        tempEncIn[j] = npub[j] ^ c[j];
    }
    blockEncryptor(k, m, &c[CRYPTO_BYTES], tempEncIn, mlen);
    return 0;
}

int crypto_aead_decrypt(
                        unsigned char *m,unsigned long long *mlen,
                        unsigned char *nsec,
                        const unsigned char *c,unsigned long long clen,
                        const unsigned char *ad,unsigned long long adlen,
                        const unsigned char *npub,
                        const unsigned char *k
                        )
    {
//    ...
//    ... the code for the AEAD implementation goes here,
//        ... generating a plaintext m[0],m[1],...,m[*mlen-1]
//        ... and secret message number nsec[0],nsec[1],...
//        ... from a ciphertext c[0],c[1],...,c[clen-1]
//        ... and associated data ad[0],ad[1],...,ad[adlen-1]
//        ... and nonce number npub[0],npub[1],...
//        ... and secret key k[0],k[1],...
//        ...
        // ciphertext is always same length as the plaintext + tag
        *mlen = clen-CRYPTO_BYTES;

        unsigned char tempEncIn[CRYPTO_BYTES];

        // set up the 'nonce' (tag + provided nonce, which could be 0x00's)
        for(int j=0;j<CRYPTO_BYTES;j++) {
            tempEncIn[j] = npub[j] ^ c[j];
        }
        
        blockEncryptor(k, c+CRYPTO_BYTES, m, tempEncIn, clen-CRYPTO_BYTES);
                
        /// check the tag ///
        
        // calculate the tag from associated data + plaintext
        unsigned char adKey[CRYPTO_BYTES];
        unsigned char tag[CRYPTO_ABYTES] = {0};
        
        // prepare round input from nonce
        for(int i =0; i<CRYPTO_BYTES;i++) {
            adKey[i] = npub[i];
        }
        // create AAD key by encrypting nonce with key
        e128Rounds(adKey, k);
        
        makeTag(adKey, ad, adlen, m, *mlen, tag);
        // check tag to see if it matches
        for(int i =0; i<CRYPTO_BYTES;i++) {
            if(c[i] != tag[i]) {
                // if tag doesn't match, return error
                return -1;
            }
        }
    return 0;
}

