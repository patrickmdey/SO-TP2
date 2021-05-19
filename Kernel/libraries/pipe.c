#include <pipe.h>
#include <memoryManager.h>
#include <staticQueue.h>

//static void insertFdNode(t_fdNode* node);
static t_fdNode* deleteFdNode(t_fdNode* fdNode, uint64_t fd, int* flag);
//static void freeRec(t_fdNode* fdNode);
static uint64_t createFd();
static void freeFd(t_fdNode* node);

static t_fdList fdList = { NULL, 0 };
static uint64_t nextFd = 0;

void initPipes() {
    createFd(); // STDIN
    createFd(); // STDOUT
}

void insertFd(t_fdNode* fdNode) {
    t_fdList* l = &fdList;
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

void pipeWrite(t_fdNode * node, char c) {
    queueInsert(node->buffer, &c);
}

void pipeWriteStr(t_fdNode * node, char * str) {
    for (int i = 0; str[i]; i++) {
        pipeWrite(node, str[i]);
    }
}

uint8_t removeFd(uint64_t fd) {
    t_fdList* l = &fdList;
    if (l == NULL)
        return -1;
    int flag = 0;
    l->first = deleteFdNode(l->first, fd, &flag);
    if (flag)
        l->size--;
    return flag;
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

static uint64_t createFd() {
    t_fdNode* node = malloc(sizeof(t_fdNode));
    node->fd = nextFd++;
    node->buffer = queueInit(sizeof(char));
    node->next = NULL;
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
