#ifndef STATIC_STACK_H
#define STATIC_STACK_H

#include <stdint.h>

typedef struct{
    void * stack;
    int size;
    int dim;
    uint64_t dataSize;
}t_stack;

void stackPop(t_stack * stack, void * elem);
void stackPush(t_stack * stack, void * elem);
int stackIsEmpty(t_stack * stack);
void stackPeek(t_stack * stack, void * elem);

#endif