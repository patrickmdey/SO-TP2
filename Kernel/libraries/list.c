#include <list.h>
#include <staticQueue.h>

static t_PCB* deletePCB(t_PCB* pcb, int pid, int* flag);
static void freeRec(t_PCB* pcb);
static void freePCB(t_PCB* pcb);

t_list* createList() {
    return malloc(sizeof(t_list));
}

void freeList(t_list* l) {
    if (l == NULL)
        return;
    freeRec(l->first);
    free((void*)l);
}

uint8_t removePCB(t_list* l, int pid) {
    if (l == NULL)
        return -1;
    int flag = 0;
    l->first = deletePCB(l->first, pid, &flag);
    if (flag)
        l->size--;
    return flag;
}


void insertPCB(t_list* l, t_PCB* pcb) {
    if (l == NULL) {
        return;
    }

    if (l->first == NULL) {
        l->first = pcb;
        l->size++;
        return;
    }

    t_PCB* current = l->first;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = pcb;
    l->size++;
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

int getSize(t_list* l) {
    if (l == NULL)
        return -1;
    return l->size;
}

static t_PCB* deletePCB(t_PCB* pcb, int pid, int* flag) {
    if (pcb == NULL) {
        return NULL;
    }

    if (pcb->pid == pid) {
        t_PCB* next = pcb->next;
        freePCB(pcb);
        *flag = 1;
        return next;
    }

    pcb->next = deletePCB(pcb->next, pid, flag);
    return pcb;
}

static void freePCB(t_PCB * pcb) {
    free(pcb->rbp);
    // destruir fd creado?
    int i, count = (pcb->argc) + 1 - pcb->foreground;
    for (i = 0; i < count; i++)
        free(pcb->argv[i]);

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