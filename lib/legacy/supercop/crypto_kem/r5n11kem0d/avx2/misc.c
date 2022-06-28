/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the miscellaneous functions.
 */

#include "misc.h"

#include <stdio.h>

void print_hex(const char *var, const unsigned char *data, const size_t nr_elements, const size_t element_size) {
    size_t i, ii;
    if (var != NULL) {
        printf("%s[%zu]=", var, nr_elements);
    }
    for (i = 0; i < nr_elements; ++i) {
        if (i > 0) {
            printf(" ");
        }
        for (ii = element_size; ii > 0; --ii) {
            printf("%02hhX", data[i * element_size + ii - 1]);
        }
    }
    if (var != NULL) {
        printf("\n");
    }
}

void print_sage_u_vector(const char *var, const uint16_t *vector, const size_t nr_elements) {
    size_t i;
    if (var != NULL) {
        printf("%s[%zu]=", var, nr_elements);
    }
    printf("[ ");
    for (i = 0; i < nr_elements; ++i) {
        if (i > 0) {
            printf(", ");
        }
        printf("%hu", vector[i]);
    }
    printf(" ]");
    if (var != NULL) {
        printf("\n");
    }
}

void print_sage_u_matrix(const char *var, const uint16_t *matrix, const size_t nr_rows, const size_t nr_columns) {
    size_t i;
    if (var != NULL) {
        printf("%s[%zu][%zu]=", var, nr_rows, nr_columns);
    }
    printf("Matrix([");
    for (i = 0; i < nr_rows; ++i) {
        if (i > 0) {
            printf(",");
        }
        if (nr_rows > 1) {
            printf("\n");
        } else {
            printf(" ");
        }
        print_sage_u_vector(NULL, matrix + i*nr_columns, nr_columns);
    }
    if (nr_rows > 1) {
        printf("\n])");
    } else {
        printf(" ])");
    }
    if (var != NULL) {
        printf("\n");
    }
}

void print_sage_u_vector_matrix(const char *var, const uint16_t *matrix, const size_t nr_rows, const size_t nr_columns, const size_t nr_elements) {
    size_t i, j;
    if (nr_elements == 1) {
        print_sage_u_matrix(var, matrix, nr_rows, nr_columns);
    } else if (nr_rows == 1 && nr_columns == 1) {
        print_sage_u_vector(var, matrix, nr_elements);
    } else {
        if (var != NULL) {
            printf("%s[%zu][%zu][%zu]=", var, nr_rows, nr_columns, nr_elements);
        }
        printf("Matrix([");
        for (i = 0; i < nr_rows; ++i) {
            if (i > 0) {
                printf(",");
            }
            if (nr_rows > 1) {
                printf("\n[");
            } else {
                printf(" [");
            }
            for (j = 0; j < nr_columns; ++j) {
                if (j > 0) {
                    printf(",");
                }
                if (nr_columns > 1 && nr_elements > 1) {
                    printf("\n  ");
                } else {
                    printf(" ");
                }
                print_sage_u_vector(NULL, matrix + (i * nr_columns + j) * nr_elements, nr_elements);
            }
            if (nr_columns > 1 && nr_elements > 1) {
                printf("\n]");
            } else {
                printf(" ]");
            }
        }
        if (nr_rows > 1) {
            printf("\n])");
        } else {
            printf(" ])");
        }
        if (var != NULL) {
            printf("\n");
        }
    }
}

void print_sage_s_vector(const char *var, const int16_t *poly, const size_t nr_elements) {
    size_t i;
    if (var != NULL) {
        printf("%s[%zu]=", var, nr_elements);
    }
    printf("[ ");
    for (i = 0; i < nr_elements; ++i) {
        if (i > 0) {
            printf(", ");
        }
        printf("%hd", poly[i]);
    }
    printf(" ]");
    if (var != NULL) {
        printf("\n");
    }
}

void print_sage_s_matrix(const char *var, const int16_t *matrix, const size_t nr_rows, const size_t nr_columns) {
    size_t i;
    if (var != NULL) {
        printf("%s[%zu][%zu]=", var, nr_rows, nr_columns);
    }
    printf("Matrix([");
    for (i = 0; i < nr_rows; ++i) {
        if (i > 0) {
            printf(",");
        }
        if (nr_rows > 1) {
            printf("\n");
        } else {
            printf(" ");
        }
        print_sage_s_vector(NULL, matrix + i*nr_columns, nr_columns);
    }
    if (nr_rows > 1) {
        printf("\n])");
    } else {
        printf(" ])");
    }
    if (var != NULL) {
        printf("\n");
    }
}

void print_sage_s_vector_matrix(const char *var, const int16_t *matrix, const size_t nr_rows, const size_t nr_columns, const size_t nr_elements) {
    size_t i, j;
    if (nr_elements == 1) {
        print_sage_s_matrix(var, matrix, nr_rows, nr_columns);
    } else if (nr_rows == 1 && nr_columns == 1) {
        print_sage_s_vector(var, matrix, nr_elements);
    } else {
        if (var != NULL) {
            printf("%s[%zu][%zu][%zu]=", var, nr_rows, nr_columns, nr_elements);
        }
        printf("Matrix([");
        for (i = 0; i < nr_rows; ++i) {
            if (i > 0) {
                printf(",");
            }
            if (nr_rows > 1) {
                printf("\n[");
            } else {
                printf(" [");
            }
            for (j = 0; j < nr_columns; ++j) {
                if (j > 0) {
                    printf(",");
                }
                if (nr_columns > 1 && nr_elements > 1) {
                    printf("\n  ");
                } else {
                    printf(" ");
                }
                print_sage_s_vector(NULL, matrix + (i * nr_columns + j) * nr_elements, nr_elements);
            }
            if (nr_columns > 1 && nr_elements > 1) {
                printf("\n]");
            } else {
                printf(" ]");
            }
        }
        if (nr_rows > 1) {
            printf("\n])");
        } else {
            printf(" ])");
        }
        if (var != NULL) {
            printf("\n");
        }
    }
}

uint32_t ceil_log2(uint32_t x) {
    uint32_t bits = 0;
    uint32_t ones = 0;

    while (x >>= 1) {
        ones += x & 0x1;
        ++bits;
    }
    if (ones > 1) { /* ceil */
        ++bits;
    }

    return bits;
}
