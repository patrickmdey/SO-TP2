// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <pcbQueue.h>
#include <staticQueue.h>
#include <stddef.h>
#include <memoryManager.h>

#include <stringLib.h>

#include <addressList.h>

static t_PCB* deletePCB(t_pcbQueue* q, t_PCB* pcb, int pid, int* flag);
static void freeRec(t_PCB* pcb);
static void freePCB(t_pcbQueue* q, t_PCB* pcb);

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
        freePCB(q, pcb);
        *flag = 1;
        return next;
    }

    pcb->next = deletePCB(q, pcb->next, pid, flag);
    if (pcb->next == NULL)
        q->last = pcb;

    return pcb;
}

static void freePCB(t_pcbQueue* q, t_PCB* pcb) {
    free(pcb->rbp);

    int i, count = (pcb->argc) + 1 - pcb->foreground;
    for (i = 0; i <= count; i++) {
        /*if (pcb->parent != NULL) {
            removeAddress(pcb->parent->addresses, pcb->argv[i]);
        }*/
        free(pcb->argv[i]);
    }

    //freeAddressList(pcb->addresses);

    t_waitingPid* current = pcb->waiting;
    t_waitingPid* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    /*current = pcb->children;
    t_PCB * child;

    while (current != NULL) {
        child = findPCB(q, current->pid);
        if (child != NULL) {
            child->parent = NULL;
        }
        next = current->next;
        free(current);
        current = next;
    }*/

    /*if (pcb->parent != NULL) {
        removeAddress(pcb->parent->addresses, pcb->argv);
        current = pcb->parent->children;
        if (current != NULL) {
            if (current->pid == pcb->pid) {
                next = current->next;
                free(current);
                pcb->parent->children = next;
            } else {
                t_waitingPid * prev = current;
                current = prev->next;
                if (current != NULL) {
                    next = current->next;
                } else {
                    next = NULL;
                }
                while (current != NULL && current->pid != pcb->pid) {
                    prev = current;
                    current = current->next;
                    if (current != NULL) {
                        next = current->next;
                    } else {
                        next = NULL;
                    }
                }
                if (current != NULL) {
                    prev->next = next;
                    free(current);
                }
            }
        }
    }*/
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