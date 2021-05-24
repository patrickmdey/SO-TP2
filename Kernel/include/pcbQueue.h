#ifndef PCB_QUEUE_H
#define PCB_QUEUE_H

#include <taskManager.h>

typedef struct t_pcbQueue {
    int size;
    t_PCB* first;
    t_PCB* last;
} t_pcbQueue;

t_pcbQueue* createPcbQueue();

void freePcbQueue(t_pcbQueue* q);

uint8_t removePCB(t_pcbQueue* q, int pid);

t_PCB* peekFirst(t_pcbQueue* q);

t_PCB* popFirst(t_pcbQueue* q);

void insertPCB(t_pcbQueue* q, t_PCB* pcb);

t_PCB* findPCB(t_pcbQueue* q, int pid);

int getSize(t_pcbQueue* q);


#endif