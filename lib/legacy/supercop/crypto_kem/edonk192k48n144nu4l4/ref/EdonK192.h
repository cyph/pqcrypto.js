#define NrRows 48
#define NrColumns 144
#define NrColumns8 NrColumns/8
#define ProjectionDim 40
#define BinarySpan 4
#define ErrorBasisDimension 4
#define DecodingFailureError 1
#define CorruptedCiphertextError 2


static const uint8_t OrthoPairsRotationsRot[][2] = {
{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 3, 2}, { 0, 0}, { 0, 0}, { 0, 0}, { 3, 3}, { 0, 0}, { 0, 0}, { 0, 0}, { 3, 4}, 
{ 0, 0}, { 5, 3}, { 0, 0}, { 3, 5}, { 0, 0}, { 0, 0}, { 0, 0}, { 3, 6}, { 0, 0}, { 0, 0}, { 0, 0}, { 3, 7}, { 0, 0}, { 5, 5}, { 0, 0}, { 3, 8}, 
{ 0, 0}, { 0, 0}, { 0, 0}, { 3, 9}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,10}, { 0, 0}, { 5, 7}, { 0, 0}, { 3,11}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,12}, 
{ 0, 0}, { 9, 5}, { 0, 0}, { 3,13}, { 0, 0}, { 5, 9}, { 0, 0}, { 3,14}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,15}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,16}, 
{ 0, 0}, { 5,11}, { 0, 0}, { 3,17}, { 0, 0}, { 9, 7}, { 0, 0}, { 3,18}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,19}, { 0, 0}, { 5,13}, { 0, 0}, { 3,20}, 
{ 0, 0}, { 0, 0}, { 0, 0}, { 3,21}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,22}, { 0, 0}, { 5,15}, { 0, 0}, { 3,23}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,24}, 
{ 0, 0}, {13, 7}, { 0, 0}, { 3,25}, { 0, 0}, { 5,17}, { 0, 0}, { 3,26}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,27}, { 0, 0}, { 9,11}, { 0, 0}, { 3,28}, 
{ 0, 0}, { 5,19}, { 0, 0}, { 3,29}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,30}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,31}, { 0, 0}, { 5,21}, { 0, 0}, { 3,32}, 
{ 0, 0}, { 9,13}, { 0, 0}, { 3,33}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,34}, { 0, 0}, { 5,23}, { 0, 0}, { 3,35}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,36}, 
{ 0, 0}, { 0, 0}, { 0, 0}, { 3,37}, { 0, 0}, { 5,25}, { 0, 0}, { 3,38}, { 0, 0}, {13,11}, { 0, 0}, { 3,39}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,40}, 
{ 0, 0}, { 5,27}, { 0, 0}, { 3,41}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,42}, { 0, 0}, { 9,17}, { 0, 0}, { 3,43}, { 0, 0}, { 5,29}, { 0, 0}, { 3,44}, 
{ 0, 0}, { 0, 0}, { 0, 0}, { 3,45}, { 0, 0}, {13,13}, { 0, 0}, { 3,46}, { 0, 0}, { 5,31}, { 0, 0}, { 3,47}, { 0, 0}, { 9,19}, { 0, 0}, { 3,48}, 
{ 0, 0}, { 0, 0}, { 0, 0}, { 3,49}, { 0, 0}, { 5,33}, { 0, 0}, { 3,50}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,51}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,52}, 
{ 0, 0}, { 5,35}, { 0, 0}, { 3,53}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,54}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,55}, { 0, 0}, { 5,37}, { 0, 0}, { 3,56}, 
{ 0, 0}, { 0, 0}, { 0, 0}, { 3,57}, { 0, 0}, { 9,23}, { 0, 0}, { 3,58}, { 0, 0}, { 5,39}, { 0, 0}, { 3,59}, { 0, 0}, {13,17}, { 0, 0}, { 3,60}, 
{ 0, 0}, {21,11}, { 0, 0}, { 3,61}, { 0, 0}, { 5,41}, { 0, 0}, { 3,62}, { 0, 0}, { 9,25}, { 0, 0}, { 3,63}, { 0, 0}, { 0, 0}, { 0, 0}, { 3,64}
};

struct PrivateKeyStruct {
    Kelt a;
    Kelt b;
    uint8_t ShortenedPermutation[ProjectionDim][NrColumns8];
};

struct PublicKeyStruct {
    Kelt    p[BinarySpan*2];
    uint8_t cosetLeader[NrRows/(8/BinarySpan)];
    uint8_t linearCombination[NrRows][NrColumns/(8/BinarySpan)];
};

struct CiphertextStruct {
    Kelt    cph[NrColumns];
    unsigned char HashedSecretShare[crypto_hash_sha384_BYTES];
};

struct PoolOfRandomBytes {
    int poolsize;
    unsigned char pool[6*NrColumns + 7*ProjectionDim];
};
