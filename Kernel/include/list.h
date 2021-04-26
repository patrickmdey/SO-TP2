#ifndef LIST_H
#define LIST_H

#include <memoryManager.h>
#include <taskManager.h>

typedef struct t_list {
    int size;
    t_PCB * first;
    t_PCB * current;
} t_list;

t_list * createList();
void insertPCB(t_list *list, t_PCB *pcb);
uint8_t removePCB(t_list *l, int pid);
t_PCB * findPCB(t_list *l, int pid);
void freeList(t_list *l);
int getSize(t_list *l);

#endif
