#include <pcbQueue.h>
#include <staticQueue.h>
#include <stddef.h>
#include <memoryManager.h>

#include <stringLib.h>

static t_PCB* deletePCB(t_pcbQueue* q, t_PCB* pcb, int pid, int* flag);
static void freeRec(t_PCB* pcb);
static void freePCB(t_PCB* pcb);

t_pcbQueue* createPcbQueue() {
    return malloc(sizeof(t_queue));
}

void freePcbQueue(t_pcbQueue* q) {
    if (q == NULL)
        return;
    freeRec(q->first);
    free((void*)q);
}

uint8_t removePCB(t_pcbQueue* q, int pid) {
    if (q == NULL)
        return -1;

    int flag = 0;
    q->first = deletePCB(q, q->first, pid, &flag);
    if (flag)
        q->size--;
    return flag;
}

t_PCB* peekFirst(t_pcbQueue* q) {
    if (q == NULL)
        return NULL;

    return q->first;
}

t_PCB* popFirst(t_pcbQueue* q) {
    if (q == NULL)
        return NULL;

    if (q->first == NULL)
        return NULL;

    t_PCB* toReturn = q->first;
    q->first = q->first->next;
    q->size--;
    return toReturn;
}

void insertPCB(t_pcbQueue* q, t_PCB* pcb) {
    if (q == NULL) {
        return;
    }

    pcb->next = NULL;

    if (q->first == NULL) {
        q->first = pcb;
        q->last = pcb;
        q->size++;
        return;
    }

    q->last->next = pcb;
    q->last = pcb;
    q->size++;
}

t_PCB* findPCB(t_pcbQueue* q, int pid) {
    if (q == NULL)
        return NULL;

    t_PCB* pcb = q->first;
    while (pcb != NULL && pcb->pid != pid) {
        pcb = pcb->next;
    }
    return pcb;
}

int getSize(t_pcbQueue* q) {
    if (q == NULL)
        return -1;
    return q->size;
}

static t_PCB* deletePCB(t_pcbQueue* q, t_PCB* pcb, int pid, int* flag) {
    if (pcb == NULL) {
        return NULL;
    }

    if (pcb->pid == pid) {
        t_PCB* next = pcb->next;
        freePCB(pcb);
        *flag = 1;
        return next;
    }

    pcb->next = deletePCB(q, pcb->next, pid, flag);
    if (pcb->next == NULL)
        q->last = pcb;

    return pcb;
}

static void freePCB(t_PCB* pcb) {
    free(pcb->rbp);

    int i, count = (pcb->argc) + 1 - pcb->foreground;
    for (i = 0; i <= count; i++)
        free(pcb->argv[i]);
        
    for (i = 0; i < pcb->dirArrayIndex; i++)
        free(pcb->dirArray[i]);
    free(pcb->dirArray);

    t_waitingPid* current = pcb->waiting;
    t_waitingPid* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(pcb->argv);
    free(pcb);
}

static void freeRec(t_PCB* pcb) {
    if (pcb == NULL)
        return;

    t_PCB* next = pcb->next;
    free(pcb);
    freeRec(next);
}