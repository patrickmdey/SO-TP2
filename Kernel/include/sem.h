#ifndef SEM_H
#define SEM_H

#include <stdint.h>
#include <stddef.h>
#include <lib.h>
#include <memoryManager.h>

typedef struct t_waitingPid {
    uint64_t pid;
    struct t_waitingPid * next;
} t_waitingPid;

typedef struct t_sem {
    char * name;
    int value;
    int chan;
    struct t_waitingPid * waiting;
} t_sem;

typedef struct t_semNode {
    t_sem * sem;
    struct t_semNode * next;
} t_semNode;

typedef struct t_semList {
    struct t_semNode * first;
    int size;
} t_semList;


t_sem * semOpen(char * name, uint8_t create);

void semInit(t_sem * sem, int value);

void semWait(t_sem * sem);

void semPost(t_sem * sem);

void semClose(t_sem * sem);

t_semList * createSemList();

void freeSemList(t_semList* l);

void insertSem(t_semList * list, t_sem * sem);

uint8_t removeSem(t_semList * l, int chan);

t_semNode * findSem(t_semList * l, int chan);

t_semNode * findSemByName(t_semList * l, char * name);

int getSize(t_semList* l);

#endif