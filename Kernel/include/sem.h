#ifndef SEM_H
#define SEM_H

#include <stdint.h>
#include <stddef.h>
#include <lib.h>
#include <memoryManager.h>
#include <waitingPid.h>

typedef struct t_sem {
    char * name;
    int value;
    int chan;
    int lock;
    t_waitingPid * waiting;
    uint64_t processAmount;
} t_sem;

typedef struct t_semNode {
    t_sem * sem;
    struct t_semNode * next;
} t_semNode;

typedef struct t_semList {
    t_semNode * first;
    int size;
} t_semList;


t_sem * semOpen(char * name, uint8_t create, uint64_t value);

void semInit(t_sem * sem, int value);

void semWait(t_sem * sem);

void semPost(t_sem * sem);

void semClose(t_sem * sem);

void semDestroy(t_sem * sem);

char** semInfo(int* index);

t_semList * createSemList();

void freeSemList(t_semList* l);

void insertSem(t_semList * list, t_sem * sem);

uint8_t removeSem(t_semList * l, int chan);

t_semNode * findSem(t_semList * l, int chan);

t_semNode * findSemByName(t_semList * l, char * name);

int getSize(t_semList* l);

#endif