//random bytes
int random_bytes(unsigned char *r, unsigned int len);
//pseudo-random bytes
int pseudo_random_bytes(unsigned char *r, unsigned int len, const unsigned char *seed);
//hash
int hash(const unsigned char *in, unsigned int len_in, unsigned char * out);
//generate seed
int gen_seed(unsigned char *in, unsigned int len_in, unsigned char * out);