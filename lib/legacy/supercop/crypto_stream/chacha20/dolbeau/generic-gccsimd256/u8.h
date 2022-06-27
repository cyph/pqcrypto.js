/*
u4.h version $Date$
D. J. Bernstein
Romain Dolbeau
Public domain.
*/

typedef unsigned int V __attribute__ ((vector_size(32)));
typedef unsigned long V64 __attribute__ ((vector_size(32)));

typedef unsigned int hV __attribute__ ((vector_size(16)));
typedef unsigned long hV64 __attribute__ ((vector_size(16)));

#define printv8i(p,v)                                                  \
  {                                                                     \
    int z;                                                              \
    printf("%8s:%8s = ",p,#v);                                          \
    for (z = 7 ; z >= 0 ; z--) {                                       \
      printf("0x%08x", ((V)v)[z]);                                        \
      if ((z%1)==0) printf(" ");                                        \
    }                                                                   \
    printf("\n");                                                       \
  }


#define VEC8_ROT(a,imm) ((a << imm) | (a >> (32-imm)))
#define VEC8_DUP(a) (V){a,a,a,a,a,a,a,a}

#define VEC8_ROT16(a) VEC8_ROT(a,16)

#define VEC8_QUARTERROUND(a,b,c,d)                                \
	x_##a = x_##a + x_##b; t_##a = x_##d ^ x_##a; x_##d = VEC8_ROT(t_##a, 16); \
	x_##c = x_##c + x_##d; t_##c = x_##b ^ x_##c; x_##b = VEC8_ROT(t_##c, 12); \
	x_##a = x_##a + x_##b; t_##a = x_##d ^ x_##a; x_##d = VEC8_ROT(t_##a,  8); \
	x_##c = x_##c + x_##d; t_##c = x_##b ^ x_##c; x_##b = VEC8_ROT(t_##c,  7)

  if (!bytes) return;
if (bytes>=512) {
  u32 in12, in13;
  V x_0 = VEC8_DUP(x[0]);
  V x_1 = VEC8_DUP(x[1]);
  V x_2 = VEC8_DUP(x[2]);
  V x_3 = VEC8_DUP(x[3]);
  V x_4 = VEC8_DUP(x[4]);
  V x_5 = VEC8_DUP(x[5]);
  V x_6 = VEC8_DUP(x[6]);
  V x_7 = VEC8_DUP(x[7]);
  V x_8 = VEC8_DUP(x[8]);
  V x_9 = VEC8_DUP(x[9]);
  V x_10 = VEC8_DUP(x[10]);
  V x_11 = VEC8_DUP(x[11]);
  V x_12;// = VEC8_DUP(x[12]); /* useless */
  V x_13;// = VEC8_DUP(x[13]); /* useless */
  V x_14 = VEC8_DUP(x[14]);
  V x_15 = VEC8_DUP(x[15]);
  V orig0 = x_0;
  V orig1 = x_1;
  V orig2 = x_2;
  V orig3 = x_3;
  V orig4 = x_4;
  V orig5 = x_5;
  V orig6 = x_6;
  V orig7 = x_7;
  V orig8 = x_8;
  V orig9 = x_9;
  V orig10 = x_10;
  V orig11 = x_11;
  V orig12;// = x_12; /* useless */
  V orig13;// = x_13; /* useless */
  V orig14 = x_14;
  V orig15 = x_15;
  V t_0;
  V t_1;
  V t_2;
  V t_3;
  V t_4;
  V t_5;
  V t_6;
  V t_7;
  V t_8;
  V t_9;
  V t_10;
  V t_11;
  V t_12;
  V t_13;
  V t_14;
  V t_15;

  while (bytes >= 512) {
    x_0 = orig0;
    x_1 = orig1;
    x_2 = orig2;
    x_3 = orig3;
    x_4 = orig4;
    x_5 = orig5;
    x_6 = orig6;
    x_7 = orig7;
    x_8 = orig8;
    x_9 = orig9;
    x_10 = orig10;
    x_11 = orig11;
    //x_12 = orig12; /* useless */
    //x_13 = orig13; /* useless */
    x_14 = orig14;
    x_15 = orig15;

    in12 = (x[12]);
    in13 = (x[13]);
    u64 in1213 = ((u64)in12) | (((u64)in13) << 32);
    x_12 = (V)
      { (unsigned int)(in1213+0)&0xFFFFFFFF,
        (unsigned int)(in1213+1)&0xFFFFFFFF,
        (unsigned int)(in1213+2)&0xFFFFFFFF,
        (unsigned int)(in1213+3)&0xFFFFFFFF,
        (unsigned int)(in1213+4)&0xFFFFFFFF,
        (unsigned int)(in1213+5)&0xFFFFFFFF,
        (unsigned int)(in1213+6)&0xFFFFFFFF,
        (unsigned int)(in1213+7)&0xFFFFFFFF };
    x_13 = (V)
      { (unsigned int)((in1213+0)>>32)&0xFFFFFFFF,
        (unsigned int)((in1213+1)>>32)&0xFFFFFFFF,
        (unsigned int)((in1213+2)>>32)&0xFFFFFFFF,
        (unsigned int)((in1213+3)>>32)&0xFFFFFFFF,
        (unsigned int)((in1213+4)>>32)&0xFFFFFFFF,
        (unsigned int)((in1213+5)>>32)&0xFFFFFFFF,
        (unsigned int)((in1213+6)>>32)&0xFFFFFFFF,
        (unsigned int)((in1213+7)>>32)&0xFFFFFFFF };

    orig12 = x_12;
    orig13 = x_13;

    in1213 += 8;
    
    x[12] = in1213 & 0xFFFFFFFF;
    x[13] = (in1213>>32)&0xFFFFFFFF;

    for (i = 0 ; i < ROUNDS ; i+=2) {
      VEC8_QUARTERROUND( 0, 4, 8,12);
      VEC8_QUARTERROUND( 1, 5, 9,13);
      VEC8_QUARTERROUND( 2, 6,10,14);
      VEC8_QUARTERROUND( 3, 7,11,15);
      VEC8_QUARTERROUND( 0, 5,10,15);
      VEC8_QUARTERROUND( 1, 6,11,12);
      VEC8_QUARTERROUND( 2, 7, 8,13);
      VEC8_QUARTERROUND( 3, 4, 9,14);
    }


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define TRANSBLOCK(a,b,c,d)			\
    t0 = (V){x_##a[0], x_##b[0], x_##c[0], x_##d[0], x_##a[4], x_##b[4], x_##c[4], x_##d[4]};			\
    t1 = (V){x_##a[1], x_##b[1], x_##c[1], x_##d[1], x_##a[5], x_##b[5], x_##c[5], x_##d[5]};			\
    t2 = (V){x_##a[2], x_##b[2], x_##c[2], x_##d[2], x_##a[6], x_##b[6], x_##c[6], x_##d[6]};			\
    t3 = (V){x_##a[3], x_##b[3], x_##c[3], x_##d[3], x_##a[7], x_##b[7], x_##c[7], x_##d[7]}
#else
#define TRANSBLOCK(a,b,c,d)			\
    t0 = (V){__builtin_bswap32(x_##a[0]), __builtin_bswap32(x_##b[0]), __builtin_bswap32(x_##c[0]), __builtin_bswap32(x_##d[0]), \
             __builtin_bswap32(x_##a[4]), __builtin_bswap32(x_##b[4]), __builtin_bswap32(x_##c[4]), __builtin_bswap32(x_##d[4])}; \
    t1 = (V){__builtin_bswap32(x_##a[1]), __builtin_bswap32(x_##b[1]), __builtin_bswap32(x_##c[1]), __builtin_bswap32(x_##d[1]), \
             __builtin_bswap32(x_##a[5]), __builtin_bswap32(x_##b[5]), __builtin_bswap32(x_##c[5]), __builtin_bswap32(x_##d[5])}; \
    t2 = (V){__builtin_bswap32(x_##a[2]), __builtin_bswap32(x_##b[2]), __builtin_bswap32(x_##c[2]), __builtin_bswap32(x_##d[2]), \
             __builtin_bswap32(x_##a[6]), __builtin_bswap32(x_##b[6]), __builtin_bswap32(x_##c[6]), __builtin_bswap32(x_##d[6])}; \
    t3 = (V){__builtin_bswap32(x_##a[3]), __builtin_bswap32(x_##b[3]), __builtin_bswap32(x_##c[3]), __builtin_bswap32(x_##d[3]), \
             __builtin_bswap32(x_##a[7]), __builtin_bswap32(x_##b[7]), __builtin_bswap32(x_##c[7]), __builtin_bswap32(x_##d[7])}
#endif

#define ONEQUAD_TRANSPOSE(a,b,c,d)                                      \
    {                                                                   \
      V t0, t1, t2, t3;							\
      x_##a = x_##a + orig##a;						\
      x_##b = x_##b + orig##b;						\
      x_##c = x_##c + orig##c;						\
      x_##d = x_##d + orig##d;						\
									\
      TRANSBLOCK(a,b,c,d);						\
									\
      hV t0l = (hV){ t0[0], t0[1], t0[2], t0[3] };			\
      hV t1l = (hV){ t1[0], t1[1], t1[2], t1[3] };			\
      hV t2l = (hV){ t2[0], t2[1], t2[2], t2[3] };			\
      hV t3l = (hV){ t3[0], t3[1], t3[2], t3[3] };			\
      hV t0h = (hV){ t0[4], t0[5], t0[6], t0[7] };\
      hV t1h = (hV){ t1[4], t1[5], t1[6], t1[7] };\
      hV t2h = (hV){ t2[4], t2[5], t2[6], t2[7] };\
      hV t3h = (hV){ t3[4], t3[5], t3[6], t3[7] };\
      t0l ^= *(hV*)(m+0);                                                 \
      t1l ^= *(hV*)(m+64);                                                \
      t2l ^= *(hV*)(m+128);                                               \
      t3l ^= *(hV*)(m+192);                                               \
      t0h ^= *(hV*)(m+256);                                                 \
      t1h ^= *(hV*)(m+320);                                                \
      t2h ^= *(hV*)(m+384);                                               \
      t3h ^= *(hV*)(m+448);                                               \
      *((hV*)(out+0)) = t0l;						\
      *((hV*)(out+64)) = t1l;						\
      *((hV*)(out+128)) = t2l;						\
      *((hV*)(out+192)) = t3l;						\
      *((hV*)(out+256)) = t0h;                                          \
      *((hV*)(out+320)) = t1h;                                          \
      *((hV*)(out+384)) = t2h;                                          \
      *((hV*)(out+448)) = t3h;                                          \
    }
    
#define ONEQUAD(a,b,c,d) ONEQUAD_TRANSPOSE(a,b,c,d)

    ONEQUAD(0,1,2,3);
    m+=16;
    out+=16;
    ONEQUAD(4,5,6,7);
    m+=16;
    out+=16;
    ONEQUAD(8,9,10,11);
    m+=16;
    out+=16;
    ONEQUAD(12,13,14,15);
    m-=48;
    out-=48;
    
#undef ONEQUAD
#undef ONEQUAD_TRANSPOSE

    bytes -= 512;
    out += 512;
    m += 512;
  }
 }
#undef VEC8_ROT
#undef VEC8_QUARTERROUND
