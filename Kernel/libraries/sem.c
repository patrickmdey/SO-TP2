#include <sem.h>
#include <utils.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <taskManager.h>

#include <stringLib.h>

static t_sem* semCreate(char* name, uint64_t value);
static void freeSem(t_semNode* l);
static void sleep(t_sem* sem);
static void wakeup(t_sem* sem);
static t_semNode* deleteSem(t_semNode* sem, int chan, int* flag);
static void freeRec(t_semNode* sem);
static int fillSemInfo(char** toReturn, int size);

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

t_semList* createSemList() {
    return malloc(sizeof(t_semList));
}

void freeSemList(t_semList* l) {
    if (l == NULL)
        return;
    freeRec(l->first);
    free((void*)l);
}

char** semInfo(int* index) {
    int size = semaphores.size;
    char** toReturn = malloc((size) * sizeof(char));

    *index = fillSemInfo(toReturn, size);
    return toReturn;

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
    }
    else {
        t_waitingPid* curr = sem->waiting;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = toCreate;
    }
    changeState(pid);
    int_20();
}

static void wakeup(t_sem* sem) {
    if (sem->waiting == NULL)
        return;

    uint64_t pid = sem->waiting->pid;
    t_waitingPid* aux = sem->waiting;
    sem->waiting = sem->waiting->next;
    changeState(pid);
    free(aux);
    (sem->value)--;
}

static int fillSemInfo(char** toReturn, int size) {
    int i = 0, j = 0;
    t_semNode* nodeIterator = semaphores.first;
    t_waitingPid* waitingIterator;
    int offset;

    while (i < size) {
        toReturn[j] = malloc(150);
        offset = 0;
        waitingIterator = nodeIterator->sem->waiting;
        offset += uintToBase(waitingIterator->pid, toReturn[j] + offset, 10);
        offset += strcpy(toReturn[j] + offset, "                 ");
        offset += strcpy(toReturn[j] + offset, nodeIterator->sem->name);
        offset += strcpy(toReturn[j] + offset, "    ");
        offset += uintToBase(nodeIterator->sem->value, toReturn[j] + offset, 10);
        offset += strcpy(toReturn[j] + offset, "       ");
        offset += strcpy(toReturn[j] + offset, nodeIterator->sem->value < 0 ? "BLOCKED     " : "UNBLOCKED");
        toReturn[j++][offset] = 0;
        for (int k = 1; k <= 3; k++) {
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