#include <sem.h>
#include <utils.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <taskManager.h>
#include <semList.h>

#include <lib.h>

#include <stringLib.h>


static t_sem* semCreate(char* name, uint64_t value);
static void sleep(t_sem* sem);
static void wakeup(t_sem* sem);
static int fillSemInfo(char** toReturn, int size);

static void acquire(t_sem* sem);
static void release(t_sem* sem);

static int chan = 0;

static t_semList semaphores = { NULL, 0 };

t_sem* semOpen(char* name, uint8_t create, uint64_t value) {
    t_semNode* sem = findSemByName(&semaphores, name);
    if (sem == NULL && create) {
        return semCreate(name, value);
    }
    else if (sem != NULL) {
        (sem->sem->processAmount)++;
        return sem->sem;
    }
    return NULL;
}

void semClose(t_sem* sem) {
    (sem->processAmount)--;
    if (sem->processAmount == 0)
        semDestroy(sem);
}

void semDestroy(t_sem* sem) {
    removeSem(&semaphores, sem->chan);
    while (sem->waiting != NULL) {
        t_waitingPid * toFree = sem->waiting;
        sem->waiting = sem->waiting->next;
        block(toFree->pid); 
        free(toFree);
    }
    free(sem);
}

void semInit(t_sem* sem, int value) {
    sem->value = value;
}

void semWait(t_sem* s) {
    if (s == NULL)
        return;

    acquire(s);

    if (s->value > 0) {
        (s->value)--;
    }
    else {
        release(s);
        sleep(s);
        // (s->value)--; <- decremento en wakup en lugar de aca
    }
    release(s);
}

void semPost(t_sem* s) {
    if (s == NULL)
        return;

    acquire(s);

    (s->value)++;
    wakeup(s);
    release(s);
}

char** semInfo(int* index) {
    int size = semaphores.size;
    t_semNode* current = semaphores.first;
    while (current != NULL) {
        size += current->sem->processAmount;
        current = current->next;
    }
    char** toReturn = malloc((size) * sizeof(char*));

    *index = fillSemInfo(toReturn, size);
    return toReturn;

}

static t_sem* semCreate(char* name, uint64_t value) {
    t_sem* sem = malloc(sizeof(t_sem));
    sem->name = name;
    sem->chan = chan++;
    sem->value = value;
    sem->waiting = NULL;
    sem->processAmount = 1;
    sem->lock = 0;
    insertSem(&semaphores, sem);
    return sem;
}

static void sleep(t_sem* sem) {
    uint64_t pid = getCurrentPid();
    t_waitingPid* toCreate = malloc(sizeof(t_waitingPid));
    toCreate->pid = pid;
    toCreate->next = NULL;
    if (sem->waiting == NULL) {
        sem->waiting = toCreate;
    }
    else {
        t_waitingPid* curr = sem->waiting;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = toCreate;
        //_xchg((uint64_t) curr->next, (uint64_t) toCreate);
    }
    block(pid);
    int_20();
}

static void wakeup(t_sem* sem) {
    if (sem->waiting == NULL)
        return;

    uint64_t pid = sem->waiting->pid;
    t_waitingPid* aux = sem->waiting;
    sem->waiting = sem->waiting->next;
    int ret = block(pid);
    free(aux);
    if (ret) {
        acquire(sem);
        (sem->value)--;
        
    }
}

//Hago un lock al semaforo para que no pueda ser incrementado ni decrementado
//mientras lo esta haciendo
//ES UNA MEDIDA DE PROTECCION AL KERNEL (que en este caso, como hay un único nucleo, no pasa)
static void acquire(t_sem* sem) {
    while (_xchg(sem->lock, 1) != 0)
        int_20();
}

static void release(t_sem* sem) {
    _xchg(sem->lock, 0);
}



static int fillSemInfo(char** toReturn, int size) {
    int i = 0, j = 0;
    t_semNode* nodeIterator = semaphores.first;
    t_waitingPid* waitingIterator;
    int offset;

    while (nodeIterator != NULL) {
        toReturn[j] = malloc(150);
        offset = 0;
        waitingIterator = nodeIterator->sem->waiting;
        if (waitingIterator != NULL) {
            offset += uintToBase(waitingIterator->pid, toReturn[j] + offset, 10);
            offset += strcpy(toReturn[j] + offset, "                 ");
            waitingIterator = waitingIterator->next;
        }
        else {
            offset += uintToBase(0, toReturn[j] + offset, 10);
            offset += strcpy(toReturn[j] + offset, "                 ");
        }
        offset += strcpy(toReturn[j] + offset, nodeIterator->sem->name);
        offset += strcpy(toReturn[j] + offset, "    ");
        offset += uintToBase(nodeIterator->sem->value, toReturn[j] + offset, 10);
        offset += strcpy(toReturn[j] + offset, "       ");
        offset += strcpy(toReturn[j] + offset, nodeIterator->sem->value == 0 ? "BLOCKED     " : "UNBLOCKED");
        toReturn[j++][offset] = 0;

        while (waitingIterator != NULL) {
            toReturn[j] = malloc(150);
            offset = 0;
            offset += uintToBase(waitingIterator->pid, toReturn[j] + offset, 10);
            toReturn[j][offset] = 0;
            j++;
            waitingIterator = waitingIterator->next;
        }
        i++;
        nodeIterator = nodeIterator->next;
    }
    return j;
}

