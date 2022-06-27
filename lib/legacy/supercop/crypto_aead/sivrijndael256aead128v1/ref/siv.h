
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "api.h"

#define STATE_INBITS (256)
#define STATE_INBYTES ((STATE_INBITS + 7) / 8)

#define KEY_INBITS (CRYPTO_KEYBYTES * 8)
#define KEY_INBYTES (CRYPTO_KEYBYTES)

#define NOUNCE_INBITS (CRYPTO_NPUBBYTES * 8)
#define NOUNCE_INBYTES (CRYPTO_NPUBBYTES)

#define TAG_INBITS (CRYPTO_ABYTES * 8)
#define TAG_INBYTES (CRYPTO_ABYTES)

#define TAG_MATCH	 0
#define TAG_UNMATCH	-1
#define OTHER_FAILURES -2