/**
 *  stack.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *
 *  This file is part of the reference implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_STACK_H
#define __NTSKEM_STACK_H

#include <stdint.h>

typedef struct node {
    void *data;
    struct node *prev;
} node;

typedef struct stack {
    node *top;
    size_t size;
} stack;

stack* stack_create();

void stack_free(stack *stack);

int stack_push(stack *stack, void *ptr);

void* stack_pop(stack *stack);

size_t stack_size(const stack *stack);

#endif /* __NTSKEM_STACK_H */
