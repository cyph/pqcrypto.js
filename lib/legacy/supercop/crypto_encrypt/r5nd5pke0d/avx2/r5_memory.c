/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the memory handling functions.
 */

#include "r5_memory.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#undef checked_malloc
#undef checked_calloc
#undef checked_realloc

extern void *checked_malloc(size_t size, const char *file, int line);
extern void *checked_calloc(size_t count, size_t size, const char *file, const int line);
extern void *checked_realloc(void *ptr, size_t size, const char *file, const int line);

int constant_time_memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t * a = s1;
    const uint8_t * b = s2;
    int ret = 0;
    size_t i;

    for (i = 0; i < n; ++i) {
        ret |= *a++ ^ *b++;
    }

    return ret;
}

void conditional_constant_time_memcpy(void * restrict dst, const void * restrict src, size_t n, uint8_t flag) {
    uint8_t * d = dst;
    const uint8_t * s = src;
    flag = (unsigned char) (-(flag | -flag) >> 7); // Force flag into 0x00 or 0xff
    size_t i;

    for (i = 0; i < n; ++i) {
        d[i] = (uint8_t) (d[i] ^ (flag & (d[i] ^ s[i])));
    }
}
