/**
 *  stack.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *
 *  This file is part of the reference implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdlib.h>
#include "stack.h"

stack* stack_create()
{
    stack *s = (stack *)calloc(1, sizeof(stack));
    if (s == NULL)
        return NULL;

    s->top = NULL;
    s->size = 0;
    
    return s;
}

void stack_free(stack *stack)
{
    if (stack) {
        while (stack->size > 0) {
            stack_pop(stack);
        }
        free(stack);
        stack = NULL;
    }
}

int stack_push(stack *stack, void *ptr)
{
    node *n = (node *)calloc(1, sizeof(node));
    if (!n)
        return 0;
    
    n->data = ptr;
    n->prev = stack->top;
    stack->top = n;
    stack->size++;
    
    return 1;
}

void* stack_pop(stack *stack)
{
    void *ptr = NULL;
    node *n = stack->top;
    if (!n)
        return NULL;
    
    ptr = n->data;
    stack->top = n->prev;
    free(n);
    
    stack->size--;
    
    return ptr;
}

size_t stack_size(const stack *stack)
{
    return stack->size;
}
