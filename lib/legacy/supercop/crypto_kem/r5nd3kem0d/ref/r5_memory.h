/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of memory handling macros and functions.
 */

#ifndef R5_MEMORY_H
#define R5_MEMORY_H

#include "misc.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Checked malloc. Abort with error message on allocation error.
     *
     * @param size the size of the memory to allocate
     * @param file the name of the file where the allocation occurred
     * @param line the line number in the file where the allocation occurred
     * @return pointer to the allocated memory
     */
    inline void *checked_malloc(size_t size, const char *file, int line) {
        (void) file; /* Prevent warning for unused parameter */
        (void) line; /* Prevent warning for unused parameter */
        void *temp = malloc(size);
        if (temp == NULL) {
            abort();
        }
        return temp;
    }

    /**
     * Checked calloc. Abort with error message on allocation error.
     *
     * @param count the number of elements to allocate
     * @param size the size of the memory to allocate
     * @param file the name of the file where the allocation occurred
     * @param line the line number in the file where the allocation occurred
     * @return pointer to the allocated memory
     */
    inline void *checked_calloc(size_t count, size_t size, const char *file, const int line) {
        (void) file; /* Prevent warning for unused parameter */
        (void) line; /* Prevent warning for unused parameter */
        void *temp = calloc(count, size);
        if (temp == NULL) {
            abort();
        }
        return temp;
    }

    /**
     * Checked realloc. Abort with error message on allocation error.
     *
     * @param ptr the pointer to the originally allocated memory
     * @param size the size of the memory to allocate instead
     * @param file the name of the file where the allocation occurred
     * @param line the line number in the file where the allocation occurred
     * @return pointer to the reallocated memory
     */
    inline void *checked_realloc(void *ptr, size_t size, const char *file, const int line) {
        (void) file; /* Prevent warning for unused parameter */
        (void) line; /* Prevent warning for unused parameter */
        void *temp = realloc(ptr, size);
        if (temp == NULL) {
            abort();
        }
        return temp;
    }

    /**
     * Constant time memory comparison function. Use to replace `memcmp()` when
     * comparing security critical data.
     *
     * @param s1 the byte string to compare to
     * @param s2 the byte string to compare
     * @param n the number of bytes to compare
     * @return 0 if all size bytes are equal, non-zero otherwise
     */
    int constant_time_memcmp(const void *s1, const void *s2, size_t n);

    /**
     * Conditionally copies the data from the source to the destination in
     * constant time.
     *
     * @param dst the destination of the copy
     * @param src the source of the copy
     * @param n the number of bytes to copy
     * @param flag indicating whether or not the copy should be performed
     */
    void conditional_constant_time_memcpy(void * restrict dst, const void * restrict src, size_t n, uint8_t flag);

#ifdef __cplusplus
}
#endif

/**
 * Wrapper for `malloc`. When an allocation error occurs, the program is
 * aborted with an error message stating the location where the allocation error
 * occurred.
 *
 * @param size the size of the memory to allocate
 * @return pointer to the allocated memory
 */
#define checked_malloc(size) checked_malloc(size, __FILE__, __LINE__)

/**
 * Wrapper for `calloc`. When an allocation error occurs, the program is
 * aborted with an error message stating the location where the allocation error
 * occurred.
 *
 * @param count the number of elements to allocate
 * @param size the size of each element
 * @return pointer to the allocated memory
 */
#define checked_calloc(count, size) checked_calloc(count, size, __FILE__, __LINE__)

/**
 * Wrapper for `realloc`. When an allocation error occurs, the program is
 * aborted with an error message stating the location where the allocation error
 * occurred.
 *
 * @param ptr the pointer to the originally allocated memory
 * @param size the size of the memory to allocate instead
 * @return pointer to the reallocated memory
 */
#define checked_realloc(ptr, size) checked_realloc(ptr, size, __FILE__, __LINE__)

#endif /* R5_MEMORY_H */
