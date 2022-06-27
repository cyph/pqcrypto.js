// hila5_sha3_opt.c
// 2017-05-07  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Implementation of FIPS-202 SHA3 hashes and SHAKE XOFs.

#include "hila5_sha3.h"
#include "hila5_endian.h"

// Keccak F function -- This version originally from Ronny Van Keer

#define ROTL64(a, offset) ((a << offset) ^ (a >> (64 - offset)))

static void hila5_sha3_keccakf(uint64_t state[25])
{
    const uint64_t KeccakF_RoundConstants[24] = {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808A,
        0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008A,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
        0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800A, 0x800000008000000A, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008
    };

    int round;

    uint64_t Aba, Abe, Abi, Abo, Abu;
    uint64_t Aga, Age, Agi, Ago, Agu;
    uint64_t Aka, Ake, Aki, Ako, Aku;
    uint64_t Ama, Ame, Ami, Amo, Amu;
    uint64_t Asa, Ase, Asi, Aso, Asu;
    uint64_t BCa, BCe, BCi, BCo, BCu;
    uint64_t Da, De, Di, Do, Du;
    uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
    uint64_t Ega, Ege, Egi, Ego, Egu;
    uint64_t Eka, Eke, Eki, Eko, Eku;
    uint64_t Ema, Eme, Emi, Emo, Emu;
    uint64_t Esa, Ese, Esi, Eso, Esu;

    HILA5_ENDIAN_FLIP64(state, 25);

    //copyFromState(A, state)
    Aba = state[0];
    Abe = state[1];
    Abi = state[2];
    Abo = state[3];
    Abu = state[4];
    Aga = state[5];
    Age = state[6];
    Agi = state[7];
    Ago = state[8];
    Agu = state[9];
    Aka = state[10];
    Ake = state[11];
    Aki = state[12];
    Ako = state[13];
    Aku = state[14];
    Ama = state[15];
    Ame = state[16];
    Ami = state[17];
    Amo = state[18];
    Amu = state[19];
    Asa = state[20];
    Ase = state[21];
    Asi = state[22];
    Aso = state[23];
    Asu = state[24];

    for (round = 0; round < 24; round += 2) {
        //    prepareTheta
        BCa = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
        BCe = Abe ^ Age ^ Ake ^ Ame ^ Ase;
        BCi = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
        BCo = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
        BCu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;

        //thetaRhoPiChiIotaPrepareTheta(round  , A, E)
        Da = BCu ^ ROTL64(BCe, 1);
        De = BCa ^ ROTL64(BCi, 1);
        Di = BCe ^ ROTL64(BCo, 1);
        Do = BCi ^ ROTL64(BCu, 1);
        Du = BCo ^ ROTL64(BCa, 1);

        Aba ^= Da;
        BCa = Aba;
        Age ^= De;
        BCe = ROTL64(Age, 44);
        Aki ^= Di;
        BCi = ROTL64(Aki, 43);
        Amo ^= Do;
        BCo = ROTL64(Amo, 21);
        Asu ^= Du;
        BCu = ROTL64(Asu, 14);
        Eba = BCa ^ ((~BCe) & BCi);
        Eba ^= (uint64_t) KeccakF_RoundConstants[round];
        Ebe = BCe ^ ((~BCi) & BCo);
        Ebi = BCi ^ ((~BCo) & BCu);
        Ebo = BCo ^ ((~BCu) & BCa);
        Ebu = BCu ^ ((~BCa) & BCe);

        Abo ^= Do;
        BCa = ROTL64(Abo, 28);
        Agu ^= Du;
        BCe = ROTL64(Agu, 20);
        Aka ^= Da;
        BCi = ROTL64(Aka, 3);
        Ame ^= De;
        BCo = ROTL64(Ame, 45);
        Asi ^= Di;
        BCu = ROTL64(Asi, 61);
        Ega = BCa ^ ((~BCe) & BCi);
        Ege = BCe ^ ((~BCi) & BCo);
        Egi = BCi ^ ((~BCo) & BCu);
        Ego = BCo ^ ((~BCu) & BCa);
        Egu = BCu ^ ((~BCa) & BCe);

        Abe ^= De;
        BCa = ROTL64(Abe, 1);
        Agi ^= Di;
        BCe = ROTL64(Agi, 6);
        Ako ^= Do;
        BCi = ROTL64(Ako, 25);
        Amu ^= Du;
        BCo = ROTL64(Amu, 8);
        Asa ^= Da;
        BCu = ROTL64(Asa, 18);
        Eka = BCa ^ ((~BCe) & BCi);
        Eke = BCe ^ ((~BCi) & BCo);
        Eki = BCi ^ ((~BCo) & BCu);
        Eko = BCo ^ ((~BCu) & BCa);
        Eku = BCu ^ ((~BCa) & BCe);

        Abu ^= Du;
        BCa = ROTL64(Abu, 27);
        Aga ^= Da;
        BCe = ROTL64(Aga, 36);
        Ake ^= De;
        BCi = ROTL64(Ake, 10);
        Ami ^= Di;
        BCo = ROTL64(Ami, 15);
        Aso ^= Do;
        BCu = ROTL64(Aso, 56);
        Ema = BCa ^ ((~BCe) & BCi);
        Eme = BCe ^ ((~BCi) & BCo);
        Emi = BCi ^ ((~BCo) & BCu);
        Emo = BCo ^ ((~BCu) & BCa);
        Emu = BCu ^ ((~BCa) & BCe);

        Abi ^= Di;
        BCa = ROTL64(Abi, 62);
        Ago ^= Do;
        BCe = ROTL64(Ago, 55);
        Aku ^= Du;
        BCi = ROTL64(Aku, 39);
        Ama ^= Da;
        BCo = ROTL64(Ama, 41);
        Ase ^= De;
        BCu = ROTL64(Ase, 2);
        Esa = BCa ^ ((~BCe) & BCi);
        Ese = BCe ^ ((~BCi) & BCo);
        Esi = BCi ^ ((~BCo) & BCu);
        Eso = BCo ^ ((~BCu) & BCa);
        Esu = BCu ^ ((~BCa) & BCe);

        //    prepareTheta
        BCa = Eba ^ Ega ^ Eka ^ Ema ^ Esa;
        BCe = Ebe ^ Ege ^ Eke ^ Eme ^ Ese;
        BCi = Ebi ^ Egi ^ Eki ^ Emi ^ Esi;
        BCo = Ebo ^ Ego ^ Eko ^ Emo ^ Eso;
        BCu = Ebu ^ Egu ^ Eku ^ Emu ^ Esu;

        //thetaRhoPiChiIotaPrepareTheta(round+1, E, A)
        Da = BCu ^ ROTL64(BCe, 1);
        De = BCa ^ ROTL64(BCi, 1);
        Di = BCe ^ ROTL64(BCo, 1);
        Do = BCi ^ ROTL64(BCu, 1);
        Du = BCo ^ ROTL64(BCa, 1);

        Eba ^= Da;
        BCa = Eba;
        Ege ^= De;
        BCe = ROTL64(Ege, 44);
        Eki ^= Di;
        BCi = ROTL64(Eki, 43);
        Emo ^= Do;
        BCo = ROTL64(Emo, 21);
        Esu ^= Du;
        BCu = ROTL64(Esu, 14);
        Aba = BCa ^ ((~BCe) & BCi);
        Aba ^= (uint64_t) KeccakF_RoundConstants[round + 1];
        Abe = BCe ^ ((~BCi) & BCo);
        Abi = BCi ^ ((~BCo) & BCu);
        Abo = BCo ^ ((~BCu) & BCa);
        Abu = BCu ^ ((~BCa) & BCe);

        Ebo ^= Do;
        BCa = ROTL64(Ebo, 28);
        Egu ^= Du;
        BCe = ROTL64(Egu, 20);
        Eka ^= Da;
        BCi = ROTL64(Eka, 3);
        Eme ^= De;
        BCo = ROTL64(Eme, 45);
        Esi ^= Di;
        BCu = ROTL64(Esi, 61);
        Aga = BCa ^ ((~BCe) & BCi);
        Age = BCe ^ ((~BCi) & BCo);
        Agi = BCi ^ ((~BCo) & BCu);
        Ago = BCo ^ ((~BCu) & BCa);
        Agu = BCu ^ ((~BCa) & BCe);

        Ebe ^= De;
        BCa = ROTL64(Ebe, 1);
        Egi ^= Di;
        BCe = ROTL64(Egi, 6);
        Eko ^= Do;
        BCi = ROTL64(Eko, 25);
        Emu ^= Du;
        BCo = ROTL64(Emu, 8);
        Esa ^= Da;
        BCu = ROTL64(Esa, 18);
        Aka = BCa ^ ((~BCe) & BCi);
        Ake = BCe ^ ((~BCi) & BCo);
        Aki = BCi ^ ((~BCo) & BCu);
        Ako = BCo ^ ((~BCu) & BCa);
        Aku = BCu ^ ((~BCa) & BCe);

        Ebu ^= Du;
        BCa = ROTL64(Ebu, 27);
        Ega ^= Da;
        BCe = ROTL64(Ega, 36);
        Eke ^= De;
        BCi = ROTL64(Eke, 10);
        Emi ^= Di;
        BCo = ROTL64(Emi, 15);
        Eso ^= Do;
        BCu = ROTL64(Eso, 56);
        Ama = BCa ^ ((~BCe) & BCi);
        Ame = BCe ^ ((~BCi) & BCo);
        Ami = BCi ^ ((~BCo) & BCu);
        Amo = BCo ^ ((~BCu) & BCa);
        Amu = BCu ^ ((~BCa) & BCe);

        Ebi ^= Di;
        BCa = ROTL64(Ebi, 62);
        Ego ^= Do;
        BCe = ROTL64(Ego, 55);
        Eku ^= Du;
        BCi = ROTL64(Eku, 39);
        Ema ^= Da;
        BCo = ROTL64(Ema, 41);
        Ese ^= De;
        BCu = ROTL64(Ese, 2);
        Asa = BCa ^ ((~BCe) & BCi);
        Ase = BCe ^ ((~BCi) & BCo);
        Asi = BCi ^ ((~BCo) & BCu);
        Aso = BCo ^ ((~BCu) & BCa);
        Asu = BCu ^ ((~BCa) & BCe);
    }

    //copyToState(state, A)
    state[0] = Aba;
    state[1] = Abe;
    state[2] = Abi;
    state[3] = Abo;
    state[4] = Abu;
    state[5] = Aga;
    state[6] = Age;
    state[7] = Agi;
    state[8] = Ago;
    state[9] = Agu;
    state[10] = Aka;
    state[11] = Ake;
    state[12] = Aki;
    state[13] = Ako;
    state[14] = Aku;
    state[15] = Ama;
    state[16] = Ame;
    state[17] = Ami;
    state[18] = Amo;
    state[19] = Amu;
    state[20] = Asa;
    state[21] = Ase;
    state[22] = Asi;
    state[23] = Aso;
    state[24] = Asu;

    HILA5_ENDIAN_FLIP64(state, 25);
}

// Initialize the context for SHA3

int hila5_sha3_init(hila5_sha3_ctx_t *c, int mdlen)
{
    int i;

    for (i = 0; i < 25; i++)
        c->st.q[i] = 0;
    c->mdlen = mdlen;
    c->rsiz = 200 - 2 * mdlen;
    c->pt = 0;

    return 1;
}

// update state with more data

int hila5_sha3_update(hila5_sha3_ctx_t *c, const void *data, size_t len)
{
    size_t i;
    int j;

    j = c->pt;
    for (i = 0; i < len; i++) {
        c->st.b[j++] ^= ((const uint8_t *) data)[i];
        if (j >= c->rsiz) {
            hila5_sha3_keccakf(c->st.q);
            j = 0;
        }
    }
    c->pt = j;

    return 1;
}

// finalize and output a hash

int hila5_sha3_final(void *md, hila5_sha3_ctx_t *c)
{
    int i;

    c->st.b[c->pt] ^= 0x06;
    c->st.b[c->rsiz - 1] ^= 0x80;
    hila5_sha3_keccakf(c->st.q);

    for (i = 0; i < c->mdlen; i++) {
        ((uint8_t *) md)[i] = c->st.b[i];
    }

    return 1;
}

// compute a SHA-3 hash (md) of given byte length from "in"

void *hila5_sha3(const void *in, size_t inlen, void *md, int mdlen)
{
    hila5_sha3_ctx_t sha3;

    // perform the hash
    hila5_sha3_init(&sha3, mdlen);
    hila5_sha3_update(&sha3, in, inlen);
    hila5_sha3_final(md, &sha3);

    // clear sensitive
    hila5_sha3_init(&sha3, 0);

    return md;
}

// SHAKE128 and SHAKE256 extensible-output functionality

void hila5_shake_xof(hila5_sha3_ctx_t *c)
{
    c->st.b[c->pt] ^= 0x1F;
    c->st.b[c->rsiz - 1] ^= 0x80;
    hila5_sha3_keccakf(c->st.q);
    c->pt = 0;
}

void hila5_shake_out(hila5_sha3_ctx_t *c, void *out, size_t len)
{
    size_t i;
    int j;

    j = c->pt;
    for (i = 0; i < len; i++) {
        if (j >= c->rsiz) {
            hila5_sha3_keccakf(c->st.q);
            j = 0;
        }
        ((uint8_t *) out)[i] = c->st.b[j++];
    }
    c->pt = j;
}

