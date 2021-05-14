#include <sem.h>
#include <utils.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <taskManager.h>

static t_sem * semCreate(char * name);
static void freeSem(t_semNode * l);
static void sleep(t_sem * sem);
static void wakeup(t_sem * sem);
static t_semNode * deleteSem(t_semNode * sem, int chan, int* flag);
static void freeRec(t_semNode* sem);

static int chan = 0;

static t_semList semaphores;

t_sem * semOpen(char * name, uint8_t create) {
    if (create) {
        return semCreate(name);
    } else {
        t_semNode * sem = findSemByName(&semaphores, name);
        if(sem != NULL)
            return sem->sem;
        
        return NULL;
    }
}

void semWait(t_sem * s) {
    if (s->value > 0) {
        s->value--;
    } else {
        sleep(s);
        s->value--;
    }
}

void semPost(t_sem * s){
    s->value++;
    wakeup(s);
}

void semInit(t_sem * sem, int value){
    sem->value = value;
}

void semClose(t_sem * sem) {
    // remove from list
    free(sem->waiting);
    free(sem);
}

t_semList* createSemList() {
    return malloc(sizeof(t_semList));
}

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


void insertSem(t_semList* l, t_sem * sem) {
    if (l == NULL) {
        return;
    }

    t_semNode * node = malloc(sizeof(t_semNode));
    node->sem = sem;

    if (l->first == NULL) {
        l->first = node;
        l->size++;
        return;
    }

    t_semNode * current = l->first;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = node;
    l->size++;
}

t_semNode * findSem(t_semList* l, int chan) {
    if (l == NULL)
        return NULL;
    t_semNode* sem = l->first;
    while (sem != NULL && sem->sem->chan != chan) {
        sem = sem->next;
    }
    return sem;
}

t_semNode * findSemByName(t_semList* l, char * name) {
    if (l == NULL)
        return NULL;
        
    t_semNode* sem = l->first;
    while (sem != NULL && stringcmp(sem->sem->name, name)) {
        sem = sem->next;
    }
    return sem;
}

static void freeSem(t_semNode * sem) {
    free(sem->sem->waiting);
    free(&(sem->sem));
    free(sem);
}

static t_semNode * deleteSem(t_semNode * sem, int chan, int* flag) {
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


static t_sem * semCreate(char * name) {
    t_sem * sem = malloc(sizeof(t_sem));
    sem->name = name;
    sem->chan = chan++;
    sem->value = 0;
    sem->waiting = malloc(sizeof(t_waitingPid *));
    insertSem(&semaphores, sem);
    return sem;
}

static void sleep(t_sem * sem) {
    uint64_t pid = getCurrentPid();
    t_waitingPid * toCreate = malloc(sizeof(t_waitingPid));
    toCreate->pid = pid;
    if (sem->waiting == NULL) {
        sem->waiting = toCreate;
    } else {
        t_waitingPid * curr = sem->waiting;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = toCreate;
    }
    changeState(pid);
    _hlt();
}

static void wakeup(t_sem * sem) {
    if(sem->waiting == NULL)
        return;
        
    uint64_t pid = sem->waiting->pid;
    sem->waiting = sem->waiting->next;
    changeState(pid);
}