#include <list.h>

static t_PCB* deletePCB(t_PCB* pcb, int pid, int* flag);
static void freeRec(t_PCB* pcb);

uint8_t* createList() {
    return malloc(sizeof(t_list));
}

void freeList(t_list* l) {
    if (l == NULL)
        return;
    freeRec(l->first);
    free((void *)l);
}

uint8_t removePCB(t_list* l, int pid) {
    if (l == NULL)
        return -1;
    int flag = 0;
    l->current = deletePCB(l->first, pid, &flag);
    if (flag)
        l->size--;
    return flag;
}


void insertPCB(t_list* l, t_PCB* pcb) {
    if (l == NULL)
        return;

    if (l->first == NULL) {
        l->first = pcb;
        return;
    }

    t_PCB* current = l->first;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = pcb;
}

t_PCB* findPCB(t_list* l, int pid) {
    if (l == NULL)
        return NULL;
    t_PCB* pcb = l->first;
    while (pcb != NULL && pcb->pid != pid) {
        pcb = pcb->next;
    }
    return pcb;
}

int getSize(t_list *l){
    if(l == NULL)
        return -1;
    return l->size;
}

static t_PCB* deletePCB(t_PCB* pcb, int pid, int* flag) {
    if (pcb == NULL)
        return pcb;

    t_PCB* prev;
    while (pcb != NULL && pcb->pid != pid) {
        prev = pcb;
        pcb = pcb->next;
    }
    if (pcb == NULL)
        *flag = -1;
    else
        prev->next = pcb->next;

    return prev;
}

static void freeRec(t_PCB* pcb) {
    if (pcb == NULL)
        return;

    t_PCB* next = pcb->next;
    free(pcb);
    freeRec(next);
}