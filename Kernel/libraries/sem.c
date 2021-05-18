#include <sem.h>
#include <utils.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <taskManager.h>
#include <semList.h>

#include <stringLib.h>


static t_sem* semCreate(char* name, uint64_t value);
static void sleep(t_sem* sem);
static void wakeup(t_sem* sem);
static int fillSemInfo(char** toReturn, int size);

static int chan = 0;

static t_semList semaphores = { NULL, 0 };

t_sem* semOpen(char* name, uint8_t create, uint64_t value) {
    t_semNode* sem = findSemByName(&semaphores, name);
    printStringLn("SEM OPEN");
    if (sem == NULL && create) {
        return semCreate(name, value);
    } else if (sem != NULL) {
        (sem->sem->processAmount)++;
        return sem->sem;
    }
    return NULL;
}

void semWait(t_sem* s) {
    if (s->value > 0) {
        (s->value)--;
    }
    else {
        sleep(s);
        // (s->value)--; <- decremento en wakup en lugar de aca
    }
}

void semPost(t_sem* s) {
    (s->value)++;
    wakeup(s);
}

void semInit(t_sem* sem, int value) {
    sem->value = value;
}

void semClose(t_sem* sem) {
    (sem->processAmount)--;
    if (sem->processAmount == 0)
        semDestroy(sem);
}

void semDestroy(t_sem* sem) {
    removeSem(&semaphores, sem->chan);
    if (sem->waiting != NULL) {
        free(sem->waiting);
    }
    free(sem);
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
    } else {
        t_waitingPid* curr = sem->waiting;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = toCreate;
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
    block(pid);
    free(aux);
    (sem->value)--;
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

