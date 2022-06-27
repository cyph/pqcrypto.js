#ifndef conv_h
#define conv_h


/* conversion function from binary data to error vectors with rank r */
void conv(const unsigned char *y, short n, short rank, gf *out_gf);

/* inverse function of the conversion function */
void inv_conv(const gf *er, short n, short rank, unsigned char *out_data);

#endif