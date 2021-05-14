#ifndef SEM_H
#define SEM_H

#include <stdint.h>

typedef struct t_sem {
    char * name;
    int value;
} t_sem;

t_sem * semOpen(char * name, uint8_t create);

void semInit(t_sem * sem, int value);

void semWait(t_sem * sem);

void semPost(t_sem * sem);

void semClose(t_sem * sem);

#endif