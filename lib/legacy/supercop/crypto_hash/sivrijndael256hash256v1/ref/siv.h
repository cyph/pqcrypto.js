
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "api.h"

#define RATE_INBITS 32
#define RATE_INBYTES ((RATE_INBITS + 7) / 8)

#define INITIAL_RATE_INBITS 128
#define INITIAL_RATE_INBYTES ((INITIAL_RATE_INBITS + 7) / 8)

#define SQUEEZE_RATE_INBITS 128
#define SQUEEZE_RATE_INBYTES ((SQUEEZE_RATE_INBITS + 7) / 8)

#define CAPACITY_INBITS 224
#define CAPACITY_INBYTES ((CAPACITY_INBITS + 7) / 8)

#define STATE_INBITS (RATE_INBITS + CAPACITY_INBITS)
#define STATE_INBYTES ((STATE_INBITS + 7) / 8)

#define TAG_INBITS (CRYPTO_BYTES * 8)
#define TAG_INBYTES (CRYPTO_BYTES)