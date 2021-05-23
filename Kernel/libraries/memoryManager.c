#include <memoryManager.h>
#include <stringLib.h>
#include <limits.h>
#include <utils.h>
#define TOTAL_MEM 1024 * 1000
#define BLOCK_SIZE 32
#define TOTAL_BLOCKS (TOTAL_MEM/BLOCK_SIZE)
uint8_t * start = (uint8_t *) 0x600000;


#ifndef BUDDY
static int findBlocks(int amount);
static void asignMemory(int firstIdx, int count, int size);

int nextindex = 0;

typedef struct block_t {
    uint16_t size;  // = a lo que pidio size = 64 
    uint8_t free;
    int pos;    //pos q tengo en el array
    uint8_t usesNext;
} block_t;

block_t blockArray[TOTAL_BLOCKS];

void initMemory() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        blockArray[i].free = 1;
        blockArray[i].pos = i;
    }
}

void * malloc(uint32_t size) {
    if (size == 0)
        return NULL;

    int amount = (size / BLOCK_SIZE);
    if (size % BLOCK_SIZE != 0)
        amount++;
        
    int firstIdx = findBlocks(amount);
    if (firstIdx == -1) {
        printStringWC("NO HAY MEMORIA", BLACK, RED);
        printStringLn("");
        return NULL;
    }

    asignMemory(firstIdx, amount, size);

    return &start[firstIdx * BLOCK_SIZE];
}

void free(void* dir) {
    if ((uint64_t) dir % BLOCK_SIZE) {
        //printStringLn("DIR NO VALIDA");
        return;
    }
    int idx = ((uint8_t *)dir - start) / BLOCK_SIZE;
    if (idx < 0 || idx >= TOTAL_BLOCKS) {
        return;
    }
    while (blockArray[idx].usesNext == 1) {
        blockArray[idx].free = 1;
        blockArray[idx].size = 0;
        blockArray[idx].usesNext = 0;
        idx++;
    }
    blockArray[idx].free = 1;
    blockArray[idx].size = 0;
}

char ** getMemoryInfo(uint64_t * size) {
    int freeMemory = 0;
    int usedMemory = 0;
    int blocksUsed = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            freeMemory++;
        } else {
            blocksUsed++;
            usedMemory += blockArray[i].size;
        }
    }
    *size = 3;
    char ** info = malloc((*size) * sizeof(char *));
    int i;
    for(i = 0; i < *size; i++) {
        info[i] = malloc(100);
    }

    int offset = 0;
    offset += strcpy(info[0], "Memoria total: ");
    offset += uintToBase(TOTAL_MEM, info[0] + offset, 10);
    offset += strcpy(info[0] + offset, " bytes");
    info[0][offset] = 0;
    offset = 0;
    offset += strcpy(info[1], "Memoria en uso: ");
    offset += uintToBase(usedMemory, info[1] + offset, 10);
    offset += strcpy(info[1] + offset, " bytes");
    info[1][offset] = 0;
    offset = 0;
    offset += strcpy(info[2], "Bloques en uso: ");
    offset += uintToBase(blocksUsed, info[2] + offset, 10);
    info[2][offset] = 0;
    return info;
}

static void asignMemory(int firstIdx, int count, int size) {
    for (int i = firstIdx; i < count + firstIdx; i++) {
        blockArray[i].size = BLOCK_SIZE;
        blockArray[i].usesNext = 1;
        blockArray[i].free = 0;
        if (i == count + firstIdx - 1) {
            if (size % BLOCK_SIZE != 0)
                blockArray[i].size = size % BLOCK_SIZE;

            blockArray[i].usesNext = 0;
        }
    }
}
static int findBlocks(int amount) {
    int i, count = 0, firstIdx;
    for (i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            if (count == 0) {
                firstIdx = i;
            }
            count++;
            if (count == amount) {
                return firstIdx;
            }
        } else {
            count = 0;
        }
    }
    return -1;
}
/*
// Otro sistema --> menor cantidad de bloques por memoria pedida
static int findBlocks(int amount) {
    //printStringLn("BUDDY!");
    int i, count = 0, firstIdx, best_fit = INT_MAX, best_fit_idx = -1;
    for (i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            if (count == 0) {
                firstIdx = i;
            }
            count++;
        }
        else {
            if (count >= amount && count < best_fit) {
                best_fit_idx = firstIdx;
                best_fit = count;
            }
            count = 0;
        }
    }

    if (count >= amount && count < best_fit) {
        return firstIdx;
    }

    return best_fit_idx;
}
*/

#else

#define BASE 2
enum powersOfTwo{K = 10, M = 20, G = 30};
#define MIN_ALLOC_LOG2 6 //Min size supported = 64 bytes 
#define MAX_ALLOC_LOG2 (M - MIN_ALLOC_LOG2) //Max size supported = 1 MB
#define BINARY_POWER(x) (1 << (x))

typedef struct blockNode_t
{
    unsigned short int order;
    unsigned short int inUse;
    struct blockNode_t *prev;
    struct blockNode_t *next;
} blockNode_t;

static void pushNode(blockNode_t *oldNode, blockNode_t *newNode);
static void deleteNode(blockNode_t *node);
static blockNode_t *popNode(blockNode_t *node);
static int isNodeEmpty(blockNode_t *node);


static unsigned int getBlock(unsigned int request);
static int getBlockToUse(unsigned int firstBlock);
static blockNode_t *findBuddy(blockNode_t *node);
static blockNode_t *getPrincipalAdress(blockNode_t *node);
static void addNodeAndMerge(blockNode_t *node);
static void addNewNode(blockNode_t *node, blockNode_t *lastNode, unsigned int order);

static blockNode_t *heapPtr;
static unsigned int heapSize;
static unsigned int cantBlocks;
static blockNode_t blocksArr[MAX_ALLOC_LOG2];
static unsigned int availableMemory;
static int usedBlocks = 0;

char ** getMemoryInfo(uint64_t * size) {
    *size = 3;
    char ** info = malloc(*size * sizeof(char *));
    int i;
    for(i = 0; i < *size; i++) {
        info[i] = malloc(100);
    }
    int offset = 0;
    offset += strcpy(info[0], "Memoria total: ");
    offset += uintToBase(heapSize, info[0] + offset, 10);
    offset += strcpy(info[0] + offset, " bytes");
    info[0][offset] = 0;
    offset = 0;
    offset += strcpy(info[1], "Memoria en uso: ");
    offset += uintToBase(heapSize - availableMemory, info[1] + offset, 10);
    offset += strcpy(info[1] + offset, " bytes");
    info[1][offset] = 0;
    offset = 0;
    offset += strcpy(info[2], "Bloques en uso: ");
    offset += uintToBase(usedBlocks, info[2] + offset, 10);
    info[2][offset] = 0;

    return info;
}

void initMemory() {

    heapPtr = (blockNode_t *) start;
    heapSize = TOTAL_MEM;
    availableMemory = TOTAL_MEM;
    cantBlocks = log2(heapSize) - MIN_ALLOC_LOG2 + 1;

    if (cantBlocks > MAX_ALLOC_LOG2)
        cantBlocks = MAX_ALLOC_LOG2;

    blockNode_t * aux= blocksArr;

    //inicializo los ordenes con sus bloques
    for (int i = 0; i < cantBlocks; i++,aux++) {
        blocksArr[i].order = i;
        blocksArr[i].inUse = 1;
        blocksArr[i].prev = blocksArr[i].next = aux;
    }
    //agrego el de mayor tamaño al heap
    addNewNode(heapPtr, &blocksArr[cantBlocks - 1], cantBlocks - 1);
}

void *malloc(uint32_t size) {
    if (size == 0) {
        return NULL;
    }
    unsigned int totalBytes = size + sizeof(blockNode_t);

    if (totalBytes > heapSize) {
        return NULL;
    }

    unsigned int block = getBlock(totalBytes);
    //obtengo el boque para el tamaño adecuado
    int parentBlock = getBlockToUse(block);
    if (parentBlock == -1) {
        return NULL;
    }

    blockNode_t *dir= popNode(&blocksArr[parentBlock]);
    //divido bloques
    while(block < parentBlock){
        dir->order--;
        addNewNode(findBuddy(dir), &blocksArr[parentBlock - 1], parentBlock - 1);
        parentBlock--;
    }
    dir->inUse = 1;
    dir++;

    availableMemory -= BINARY_POWER(block + MIN_ALLOC_LOG2);
    usedBlocks++;
    return (void *)dir;
}

void free(void *dir) {
    
    if (dir == NULL)
        return;
    blockNode_t *blockNode = (blockNode_t *)dir - 1;
    blockNode->inUse = 0;
    availableMemory += BINARY_POWER(blockNode->order + MIN_ALLOC_LOG2);
    //como devuelvo el bloque, lo mergeo con su buddy
    addNodeAndMerge(blockNode);
    usedBlocks--;
    return;
}


static void pushNode(blockNode_t *oldNode, blockNode_t *newNode) {
    blockNode_t *prev = oldNode->prev;
    newNode->prev = prev;
    newNode->next = oldNode;
    prev->next = newNode;
    oldNode->prev = newNode;
}

static void deleteNode(blockNode_t *node) {
    blockNode_t *prev = node->prev;
    blockNode_t *next = node->next;

    prev->next = next;
    next->prev = prev;
}

static blockNode_t *popNode(blockNode_t *node) {
    blockNode_t *aux = node->prev;

    if (aux == node)
        return NULL;

    deleteNode(aux);

    return aux;
}

static int isNodeEmpty(blockNode_t *node) {
    return node->prev == node;
}

static void addNewNode(blockNode_t *node, blockNode_t *lastNode, unsigned int order) {
    node->inUse = 0;
    node->order = order;
    pushNode(lastNode, node);
}

//para utilizar en el free y mergear bloque con su buddy
static void addNodeAndMerge(blockNode_t *node) {
    
    blockNode_t *buddy = findBuddy(node);
    while (node->order != cantBlocks - 1 && buddy->inUse == 0 && buddy->order == node->order) {
        
        deleteNode(buddy);

        node = getPrincipalAdress(node);
        node->order++;

        buddy = findBuddy(node);
    }
    pushNode(&blocksArr[node->order], node);
}

static blockNode_t *findBuddy(blockNode_t *node) {

    uint64_t currOffset = (uint64_t)node - (uint64_t)heapPtr;
    uint64_t newOffset = currOffset ^ BINARY_POWER(MIN_ALLOC_LOG2 + node->order);
    return (blockNode_t *)((uint64_t)heapPtr + newOffset);
}

static blockNode_t *getPrincipalAdress(blockNode_t *node) {
    
    uint64_t msk = BINARY_POWER(node->order + MIN_ALLOC_LOG2);
    msk = ~msk;

    uint64_t currentOffset = (uint64_t)node - (uint64_t)heapPtr;
    uint64_t newOffset = currentOffset & msk;
    return (blockNode_t *)((uint64_t)heapPtr + newOffset);
}

static unsigned int getBlock(unsigned int req) {
    
    unsigned int aux = log2(req);
    if (aux < MIN_ALLOC_LOG2)
        return 0;
    aux -= MIN_ALLOC_LOG2;
    return isPower(BASE, req) == 0 ? aux + 1 : aux;
}

static int getBlockToUse(unsigned int firstBlock) {

    int currentBlock = firstBlock;
    while (currentBlock < cantBlocks && isNodeEmpty(&blocksArr[currentBlock]))
        currentBlock++;
    if (currentBlock == cantBlocks)
        return -1;
    return currentBlock;
}

#endif
