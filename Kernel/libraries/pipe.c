// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <pipe.h>
#include <memoryManager.h>
#include <staticQueue.h>
#include <interrupts.h>
#include <taskManager.h>

#include <utils.h>

//static void insertFdNode(t_fdNode* node);
static t_fdNode* deleteFdNode(t_fdNode* fdNode, uint64_t fd, int* flag);
//static void freeRec(t_fdNode* fdNode);
static uint64_t createFd();
static void freeFd(t_fdNode* node);
static int fillPipeInfo(char** toReturn, int size);

static t_fdList fdList = { NULL, 0 };
static uint64_t nextFd = 0;

void initPipes() {
    createFd(); // STDIN
    createFd(); // STDOUT
}

void insertFd(t_fdNode* fdNode) {
    t_fdList * l = &fdList;
    if (l == NULL) {
        return;
    }

    if (l->first == NULL) {
        l->first = fdNode;
        l->size++;
        return;
    }

    t_fdNode* current = l->first;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = fdNode;
    l->size++;
}

void pipeWrite(t_fdNode* node, char c) {
    if (node == NULL)
        return;

    if (node->waiting != NULL) {
        t_waitingPid* toFree = node->waiting;
        block(toFree->pid);
        node->waiting = toFree->next;
        free(toFree);
    }

    queueInsert(node->buffer, &c);
}

void pipeWriteStr(t_fdNode* node, char* str) {
    if (node == NULL)
        return;

    for (int i = 0; str[i]; i++) {
        pipeWrite(node, str[i]);
    }
}

char pipeRead(uint64_t fd) {
    t_fdNode* node = findFd(fd);
    if (node == NULL)
        return 0;

    char key;
    queueRemoveData(node->buffer, &key);
    if (key != 0)
        return key;

    int pid = getCurrentPid();
    t_waitingPid* toAdd = malloc(sizeof(t_waitingPid));
    if (toAdd == NULL)
        return 0;
    
    toAdd->pid = pid;
    toAdd->next = NULL;
    t_waitingPid* curr = node->waiting;


    if (curr == NULL) {
        node->waiting = toAdd;
    }
    else {
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = toAdd;
    }
    block(pid);

    int_20();
    queueRemoveData(node->buffer, &key);
    return key;

}

void closeFd(uint64_t fd) {
    t_fdList* l = &fdList;
    if (l == NULL)
        return;
    int flag = 0;
    l->first = deleteFdNode(l->first, fd, &flag);
    if (flag)
        l->size--;
}

t_fdNode* findFd(uint64_t fd) {
    t_fdList* l = &fdList;
    if (l == NULL)
        return NULL;
    t_fdNode* fdNode = l->first;
    while (fdNode != NULL && fdNode->fd != fd) {
        fdNode = fdNode->next;
    }
    return fdNode;
}

uint64_t getFdSize() {
    return fdList.size;
}

int64_t getFd() {
    return createFd();
}

char** pipeInfo(int* index) {
    int size = fdList.size;
    t_fdNode* current = fdList.first;
    while (current != NULL) {
        t_waitingPid* pid = current->waiting;
        while (pid != NULL) {
            size++;
            pid = pid->next;
        }
        current = current->next;
    }
    char** toReturn = malloc((size) * sizeof(char*));
    if (toReturn == NULL) {
        *index = 0;
        return NULL;
    }

    *index = fillPipeInfo(toReturn, size);
    if (*index == 0)
        return NULL;
    return toReturn;

}

static uint64_t createFd() {
    t_fdNode* node = malloc(sizeof(t_fdNode));
    if (node == NULL)
        return -1;
    
    node->fd = nextFd++;
    node->buffer = queueInit(sizeof(char));
    node->next = NULL;
    node->waiting = NULL;
    insertFd(node);
    return node->fd;
}

static t_fdNode* deleteFdNode(t_fdNode* fdNode, uint64_t fd, int* flag) {
    if (fdNode == NULL) {
        return NULL;
    }

    if (fdNode->fd == fd) {
        t_fdNode* next = fdNode->next;
        freeFd(fdNode);
        *flag = 1;
        return next;
    }

    fdNode->next = deleteFdNode(fdNode->next, fd, flag);
    return fdNode;
}

static void freeFd(t_fdNode* node) {
    if (node == NULL)
        return;

    freeQueue(node->buffer);
    free(node);
}

static int fillPipeInfo(char** toReturn, int size) {
    int i = 0, j = 0;
    t_fdNode* nodeIterator = fdList.first;
    t_waitingPid* waitingIterator;
    int offset;

    while (nodeIterator != NULL) {
        toReturn[j] = malloc(150);
        if (toReturn[j] == NULL) {
            int k;
            for (k = 0; k < j; k++) {
                free(toReturn[k]);
            }
            free(toReturn);
            return 0;
        }

        offset = 0;
        waitingIterator = nodeIterator->waiting;
        if (waitingIterator != NULL) {
            offset += uintToBase(waitingIterator->pid, toReturn[j] + offset, 10);
            offset += strcpy(toReturn[j] + offset, "                 ");
            waitingIterator = waitingIterator->next;
        }
        else {
            offset += uintToBase(0, toReturn[j] + offset, 10);
            offset += strcpy(toReturn[j] + offset, "                 ");
        }
        offset += uintToBase(nodeIterator->fd, toReturn[j] + offset, 10);
        toReturn[j++][offset] = 0;

        while (waitingIterator != NULL) {
            toReturn[j] = malloc(150);
            if (toReturn[j] == NULL) {
                int k;
                for (k = 0; k < j; k++) {
                    free(toReturn[k]);
                }
                free(toReturn);
                return 0;
            }
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
