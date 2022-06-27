/*
TGIF BC implementation
Prepared by: Siang Meng Sim
Email: crypto.s.m.sim@gmail.com
Date: 25 Feb 2019
*/
#include <stdint.h>
#include "bc.h"

const uint16_t RC[] = {
/*7-bit LFSR x6,x5,x4,x3,x2,x1,x0 <- x5,x4,x3,x2,x1,x0+x6,x6*/
0x8001, 0x8002, 0x8004, 0x8008, 0x8010, 0x8020, 0x8040, 0x8003,
0x8006, 0x800C, 0x8018, 0x8030, 0x8060, 0x8043, 0x8005, 0x800A,
0x8014, 0x8028, 0x8050, 0x8023, 0x8046, 0x800F, 0x801E, 0x803C,
0x8078, 0x8073, 0x8065, 0x8049, 0x8011, 0x8022, 0x8044, 0x800B,
0x8016, 0x802C, 0x8058, 0x8033, 0x8066, 0x804F, 0x801D, 0x803A,
0x8074, 0x806B, 0x8055, 0x8029, 0x8052, 0x8027, 0x804E, 0x801F,
0x803E, 0x807C, 0x807B, 0x8075, 0x8069, 0x8051, 0x8021, 0x8042,
0x8007, 0x800E, 0x801C, 0x8038, 0x8070, 0x8063, 0x8045, 0x8009,
0x8012, 0x8024, 0x8048, 0x8013, 0x8026, 0x804C, 0x801B, 0x8036,
0x806C, 0x805B, 0x8035, 0x806A, 0x8057, 0x802D, 0x805A, 0x8037
};

void giftb64_4r(uint16_t P[4], int step, const uint16_t *RK, uint16_t C[4]){
    uint16_t S[4],T;

    S[0] = P[0];
    S[1] = P[1];
    S[2] = P[2];
    S[3] = P[3];

    /*fully unroll*/
    /*==========
    round 1
    ==========*/
    /*R1===AddRoundKey===*/
    S[0] ^= RK[8*step + 0];
    S[1] ^= RK[8*step + 1];
    S[3] ^= RC[4*step + 0];

    /*===SubCells===*/
    S[1] ^= S[0] & S[2];
    S[0] ^= S[1] & S[3];
    S[2] ^= S[0] | S[1];
    S[3] ^= S[2];
    S[1] ^= S[3];
    S[3] ^= 0xffff;
    S[2] ^= S[0] & S[1];

    T = S[0];
    S[0] = S[3];
    S[3] = T;

    /*===BitPerm1===*/
    S[1] = (S[1]&0xeeee)>>1 | (S[1]&0x1111)<<3;
    S[2] = (S[2]&0xcccc)>>2 | (S[2]&0x3333)<<2;
    S[3] = (S[3]&0x8888)>>3 | (S[3]&0x7777)<<1;



    /*==========
    round 2
    ==========*/
    /*R2===AddRoundKey===*/
    S[0] ^= RK[8*step + 2];
    S[1] ^= RK[8*step + 3];
    S[3] ^= RC[4*step + 1];

    /*===SubCells===*/
    S[1] ^= S[0] & S[2];
    S[0] ^= S[1] & S[3];
    S[2] ^= S[0] | S[1];
    S[3] ^= S[2];
    S[1] ^= S[3];
    S[3] ^= 0xffff;
    S[2] ^= S[0] & S[1];

    T = S[0];
    S[0] = S[3];
    S[3] = T;

    /*===BitPerm2===*/
    S[1] = (S[1]<<12) | (S[1]>> 4);
    S[2] = (S[2]<< 8) | (S[2]>> 8);
    S[3] = (S[3]<< 4) | (S[3]>>12);



    /*==========
    round 3
    ==========*/
    /*R3===AddRoundKey===*/
    S[0] ^= RK[8*step + 4];
    S[1] ^= RK[8*step + 5];
    S[3] ^= RC[4*step + 2];

    /*===SubCells===*/
    S[1] ^= S[0] & S[2];
    S[0] ^= S[1] & S[3];
    S[2] ^= S[0] | S[1];
    S[3] ^= S[2];
    S[1] ^= S[3];
    S[3] ^= 0xffff;
    S[2] ^= S[0] & S[1];

    T = S[0];
    S[0] = S[3];
    S[3] = T;

    /*===BitPerm3===*/
    S[1] = (S[1]&0x8888)>>3 | (S[1]&0x7777)<<1;
    S[2] = (S[2]&0xcccc)>>2 | (S[2]&0x3333)<<2;
    S[3] = (S[3]&0xeeee)>>1 | (S[3]&0x1111)<<3;



    /*==========
    round 4
    ==========*/
    /*R4===AddRoundKey===*/
    S[0] ^= RK[8*step + 6];
    S[1] ^= RK[8*step + 7];
    S[3] ^= RC[4*step + 3];

    /*===SubCells===*/
    S[1] ^= S[0] & S[2];
    S[0] ^= S[1] & S[3];
    S[2] ^= S[0] | S[1];
    S[3] ^= S[2];
    S[1] ^= S[3];
    S[3] ^= 0xffff;
    S[2] ^= S[0] & S[1];

    T = S[0];
    S[0] = S[3];
    S[3] = T;

    /*===BitPerm4===*/
    S[1] = (S[1]<< 4) | (S[1]>>12);
    S[2] = (S[2]<< 8) | (S[2]>> 8);
    S[3] = (S[3]<<12) | (S[3]>> 4);

    C[0] = S[0];
    C[1] = S[1];
    C[2] = S[2];
    C[3] = S[3];

return;}


void giftb64_4r_inv(uint16_t C[4], int step, const uint16_t *RK, uint16_t P[4]){
    uint16_t S[4],T;

    S[0] = C[0];
    S[1] = C[1];
    S[2] = C[2];
    S[3] = C[3];

    /*fully unroll*/
    /*==========
    round 4
    ==========*/
    /*===BitPerm4_inv===*/
    S[1] = (S[1]<<12) | (S[1]>> 4);
    S[2] = (S[2]<< 8) | (S[2]>> 8);
    S[3] = (S[3]<< 4) | (S[3]>>12);

    /*===SubCells_inv===*/
    T = S[3];
    S[3] = S[0];
    S[0] = T;

    S[2] ^= S[0] & S[1];
    S[3] ^= 0xffff;
    S[1] ^= S[3];
    S[3] ^= S[2];
    S[2] ^= S[0] | S[1];
    S[0] ^= S[1] & S[3];
    S[1] ^= S[0] & S[2];
    /*R4===AddRoundKey===*/
    S[0] ^= RK[8*step + 6];
    S[1] ^= RK[8*step + 7];
    S[3] ^= RC[4*step + 3];



    /*==========
    round 3
    ==========*/
    /*===BitPerm3_inv===*/
    S[1] = (S[1]&0xeeee)>>1 | (S[1]&0x1111)<<3;
    S[2] = (S[2]&0xcccc)>>2 | (S[2]&0x3333)<<2;
    S[3] = (S[3]&0x8888)>>3 | (S[3]&0x7777)<<1;

    /*===SubCells_inv===*/
    T = S[3];
    S[3] = S[0];
    S[0] = T;

    S[2] ^= S[0] & S[1];
    S[3] ^= 0xffff;
    S[1] ^= S[3];
    S[3] ^= S[2];
    S[2] ^= S[0] | S[1];
    S[0] ^= S[1] & S[3];
    S[1] ^= S[0] & S[2];

    /*R3===AddRoundKey===*/
    S[0] ^= RK[8*step + 4];
    S[1] ^= RK[8*step + 5];
    S[3] ^= RC[4*step + 2];



    /*==========
    round 2
    ==========*/
    /*===BitPerm2_inv===*/
    S[1] = (S[1]<< 4) | (S[1]>>12);
    S[2] = (S[2]<< 8) | (S[2]>> 8);
    S[3] = (S[3]<<12) | (S[3]>> 4);

    /*===SubCells_inv===*/
    T = S[3];
    S[3] = S[0];
    S[0] = T;

    S[2] ^= S[0] & S[1];
    S[3] ^= 0xffff;
    S[1] ^= S[3];
    S[3] ^= S[2];
    S[2] ^= S[0] | S[1];
    S[0] ^= S[1] & S[3];
    S[1] ^= S[0] & S[2];

    /*R2===AddRoundKey===*/
    S[0] ^= RK[8*step + 2];
    S[1] ^= RK[8*step + 3];
    S[3] ^= RC[4*step + 1];



    /*==========
    round 1
    ==========*/
    /*===BitPerm1_inv===*/
    S[1] = (S[1]&0x8888)>>3 | (S[1]&0x7777)<<1;
    S[2] = (S[2]&0xcccc)>>2 | (S[2]&0x3333)<<2;
    S[3] = (S[3]&0xeeee)>>1 | (S[3]&0x1111)<<3;

    /*===SubCells_inv===*/
    T = S[3];
    S[3] = S[0];
    S[0] = T;

    S[2] ^= S[0] & S[1];
    S[3] ^= 0xffff;
    S[1] ^= S[3];
    S[3] ^= S[2];
    S[2] ^= S[0] | S[1];
    S[0] ^= S[1] & S[3];
    S[1] ^= S[0] & S[2];

    /*R1===AddRoundKey===*/
    S[0] ^= RK[8*step + 0];
    S[1] ^= RK[8*step + 1];
    S[3] ^= RC[4*step + 0];


    P[0] = S[0];
    P[1] = S[1];
    P[2] = S[2];
    P[3] = S[3];

return;}

void TGIF_KS(const uint8_t K[16], int total_step, uint16_t *RK){
    int i;
    uint32_t TK[4], tmp;

    TK[0] = (K[ 3]<<24) | (K[ 2]<<16) | (K[ 1]<<8) | K[ 0];
    TK[1] = (K[ 7]<<24) | (K[ 6]<<16) | (K[ 5]<<8) | K[ 4];
    TK[2] = (K[11]<<24) | (K[10]<<16) | (K[ 9]<<8) | K[ 8];
    TK[3] = (K[15]<<24) | (K[14]<<16) | (K[13]<<8) | K[12];

    for(i=0; i<total_step*4; i++){
        RK[2*i + 0] = TK[0];
        RK[2*i + 1] = (TK[0]>>16);

        tmp = TK[0] ^ TK[1] ^ ((TK[3]<<8)|(TK[3]>>24)) ^ 0x00000101;
        TK[0] = TK[1];
        TK[1] = TK[2];
        TK[2] = TK[3];
        TK[3] = tmp;
    }
return;}

void left_rot(uint16_t S[4], int lrot){
    uint16_t tmp;
    while(lrot>16){
        tmp = S[0];
        S[0] = S[1];
        S[1] = S[2];
        S[2] = S[3];
        S[3] = tmp;
        lrot -= 16;
    }
    tmp = S[0]>>(16-lrot);
    S[0] = S[0]<<lrot | S[1]>>(16-lrot);
    S[1] = S[1]<<lrot | S[2]>>(16-lrot);
    S[2] = S[2]<<lrot | S[3]>>(16-lrot);
    S[3] = S[3]<<lrot | tmp;
return;}

void swap_branch(uint16_t A[4], uint16_t B[4]){
    uint16_t tmp[4];
    int i;
    for(i=0; i<4; i++){
        tmp[i] = A[i];
        A[i] = B[i];
        B[i] = tmp[i];
    }
return;}

void copy_branch(uint16_t A[4], uint16_t Acopy[4]){
    int i;
    for(i=0; i<4; i++){
        Acopy[i] = A[i];
    }
return;}

void XOR_branch(uint16_t A[4], uint16_t BplusA[4]){
    int i;
    for(i=0; i<4; i++){
        BplusA[i] ^= A[i];
    }
return;}

void TGIF(uint8_t P[16], int total_step, const uint16_t *RK, int LEFT_ROTATION, uint8_t C[16]){
    uint16_t L[4], R[4], tmp[4];
    int step;

    L[0] = P[ 1]<<8 | P[ 0];
    L[1] = P[ 3]<<8 | P[ 2];
    L[2] = P[ 5]<<8 | P[ 4];
    L[3] = P[ 7]<<8 | P[ 6];

    R[0] = P[ 9]<<8 | P[ 8];
    R[1] = P[11]<<8 | P[10];
    R[2] = P[13]<<8 | P[12];
    R[3] = P[15]<<8 | P[14];

    for(step=0; step<total_step; step++){
        giftb64_4r(R,step,RK,R);
        copy_branch(L, tmp);
        left_rot(tmp, LEFT_ROTATION);
        XOR_branch(tmp,R);
        swap_branch(L,R);
    }

    C[ 0] = L[0];
    C[ 1] = L[0]>>8;
    C[ 2] = L[1];
    C[ 3] = L[1]>>8;
    C[ 4] = L[2];
    C[ 5] = L[2]>>8;
    C[ 6] = L[3];
    C[ 7] = L[3]>>8;
    C[ 8] = R[0];
    C[ 9] = R[0]>>8;
    C[10] = R[1];
    C[11] = R[1]>>8;
    C[12] = R[2];
    C[13] = R[2]>>8;
    C[14] = R[3];
    C[15] = R[3]>>8;

return;}

void TGIF_inv(uint8_t C[16], int total_step, const uint16_t *RK, int LEFT_ROTATION, uint8_t P[16]){
    uint16_t L[4], R[4], tmp[4];
    int step;

    L[0] = C[ 1]<<8 | C[ 0];
    L[1] = C[ 3]<<8 | C[ 2];
    L[2] = C[ 5]<<8 | C[ 4];
    L[3] = C[ 7]<<8 | C[ 6];

    R[0] = C[ 9]<<8 | C[ 8];
    R[1] = C[11]<<8 | C[10];
    R[2] = C[13]<<8 | C[12];
    R[3] = C[15]<<8 | C[14];

    for(step=total_step-1; step>=0; step--){
        swap_branch(L,R);
        copy_branch(L, tmp);
        left_rot(tmp, LEFT_ROTATION);
        XOR_branch(tmp,R);
        giftb64_4r_inv(R,step,RK,R);
   }

    P[ 0] = L[0];
    P[ 1] = L[0]>>8;
    P[ 2] = L[1];
    P[ 3] = L[1]>>8;
    P[ 4] = L[2];
    P[ 5] = L[2]>>8;
    P[ 6] = L[3];
    P[ 7] = L[3]>>8;
    P[ 8] = R[0];
    P[ 9] = R[0]>>8;
    P[10] = R[1];
    P[11] = R[1]>>8;
    P[12] = R[2];
    P[13] = R[2]>>8;
    P[14] = R[3];
    P[15] = R[3]>>8;

return;}

void TGIF_BC(uint8_t P[16], const uint8_t K[16], uint8_t C[16], int encryption){
    int total_step=18;
    int left_rotation = 55;

    uint16_t RK[18*8] = {};

    TGIF_KS(K,total_step,RK);

    if(encryption)  TGIF(P,total_step,RK,left_rotation,C);
    else            TGIF_inv(C,total_step,RK,left_rotation,P);
return;}

void block_cipher (unsigned char* input, const unsigned char* userkey){
    uint8_t P[16],C[16],K[16];
    int i;
    for(i=0;i<16;i++) {
        P[i] = input[i];
        K[i] = userkey[i];
    }

    TGIF_BC(P,K,C,1);

    for(i=0;i<16;i++) {
        input[i] = C[i];
    }

};
