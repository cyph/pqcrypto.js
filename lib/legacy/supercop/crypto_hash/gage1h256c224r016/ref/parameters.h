
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#define CAPACITY                        224                         /* Sponge capacity in bits */
#define RATE                            16                          /* Sponge rate in bits */
#define RATE_BYTES                      (RATE/8)                    /* Sponge rate in bytes */
#define INTERNAL_STATE_SIZE             (CAPACITY + RATE)           /* Sponge size in bits */
#define INTERNAL_STATE_SIZE_BYTES       (INTERNAL_STATE_SIZE/8)     /* Sponge size in bytes */
#define MAX_ROUNDS                      (INTERNAL_STATE_SIZE/2)     /* The maximal recommended number of rounds */

/* Uncomment the following line for an ultraconservative choice of number of rounds in the sponge permutation */
// #define ROUNDS                           MAX_ROUNDS                  /* This is ultra conservative choice or rounds */
#define ROUNDS                          32

#endif /* PARAMETERS_H_ */

