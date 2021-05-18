#include <semList.h>
#include <memoryManager.h>
#include <utils.h>

static void freeSem(t_semNode* sem);
static t_semNode* deleteSem(t_semNode* sem, int chan, int* flag);
static void freeRec(t_semNode* sem);

void freeSemList(t_semList* l) {
    if (l == NULL)
        return;
    freeRec(l->first);
    free((void*)l);
}

uint8_t removeSem(t_semList* l, int chan) {
    if (l == NULL)
        return -1;
    int flag = 0;
    l->first = deleteSem(l->first, chan, &flag);
    if (flag)
        l->size--;
    return flag;
}

void insertSem(t_semList* l, t_sem* sem) {
    if (l == NULL) {
        return;
    }

    t_semNode* node = malloc(sizeof(t_semNode));
    node->sem = sem;
    node->next = NULL;

    if (l->first == NULL) {
        l->first = node;
        l->size++;
        return;
    }

    t_semNode* current = l->first;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = node;
    l->size++;
}

t_semNode* findSem(t_semList* l, int chan) {
    if (l == NULL)
        return NULL;
    t_semNode* sem = l->first;
    while (sem != NULL && sem->sem->chan != chan) {
        sem = sem->next;
    }
    return sem;
}

t_semNode* findSemByName(t_semList* l, char* name) {
    if (l == NULL)
        return NULL;

    t_semNode* sem = l->first;
    while (sem != NULL && stringcmp(sem->sem->name, name)) {
        sem = sem->next;
    }
    return sem;
}

static void freeSem(t_semNode* sem) {
    free(sem->sem->waiting);
    free(&(sem->sem));
    free(sem);
}

static t_semNode* deleteSem(t_semNode* sem, int chan, int* flag) {
    if (sem == NULL) {
        return NULL;
    }

    if (sem->sem->chan == chan) {
        t_semNode* next = sem->next;
        freeSem(sem);
        *flag = 1;
        return next;
    }

    sem->next = deleteSem(sem->next, chan, flag);
    return sem;
}

static void freeRec(t_semNode* sem) {
    if (sem == NULL)
        return;

    t_semNode* next = sem->next;
    free(sem);
    freeRec(next);
}
