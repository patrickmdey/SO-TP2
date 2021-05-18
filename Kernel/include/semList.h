#ifndef SEM_LIST_H
#define SEM_LIST_H

#include <sem.h>

void freeSemList(t_semList* l);

uint8_t removeSem(t_semList* l, int chan);

void insertSem(t_semList* l, t_sem* sem);

t_semNode* findSem(t_semList* l, int chan);

t_semNode* findSemByName(t_semList* l, char* name);

#endif