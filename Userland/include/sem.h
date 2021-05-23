#ifndef SEM_H
#define SEM_H

#include <stdint.h>

typedef struct t_sem {
    char * name;
    int value;
    int chan;
    struct t_waitingPid * waiting;
    uint64_t processAmount;
} t_sem;

t_sem * semOpen(char * name, uint8_t create, uint64_t value);

void semInit(t_sem * sem, int value);

void semWait(t_sem * sem);

void semPost(t_sem * sem);

void semClose(t_sem * sem);

void semDestroy(t_sem * sem);

#endif