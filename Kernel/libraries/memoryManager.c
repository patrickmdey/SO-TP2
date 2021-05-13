#include <memoryManager.h>
#include <stringLib.h>
#include <limits.h>
#include <utils.h>

#define TOTAL_MEM 1024 * 1000
#define BLOCK_SIZE 64
#define TOTAL_BLOCKS (TOTAL_MEM/BLOCK_SIZE)

static int findBlocks(int amount);
static void asignMemory(int firstIdx, int count, int size);

uint8_t * start = (uint8_t *) 0x600000;
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

void* malloc(uint32_t size) {
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
    int idx = ((uint8_t*)dir - start) / BLOCK_SIZE;
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
        }
        else {
            blocksUsed++;
            usedMemory += blockArray[i].size;
        }
    }
    *size = 3;
    char ** info = malloc(*size * sizeof(char *));
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
    offset += uintToBase(usedMemory * BLOCK_SIZE, info[1] + offset, 10);
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
        if (i == count + firstIdx - 1) {
            if (size % BLOCK_SIZE != 0)
                blockArray[i].size = size % BLOCK_SIZE;

            blockArray[i].usesNext = 0;
        }
        blockArray[i].free = 0;
    }
}

#ifndef BUDDY

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
        }
    }
    return -1;
}

#else
// Otro sistema --> menor cantidad de bloques por memoria pedida
static int findBlocks(int amount) {
    printStringLn("BUDDY!");
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

#endif