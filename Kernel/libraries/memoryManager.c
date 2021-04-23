#include <memoryManager.h>
#include <stringLib.h>
#include <limits.h>

#define TOTAL_MEM 1024 * 1000
#define BLOCK_SIZE 64
#define TOTAL_BLOCKS (TOTAL_MEM/BLOCK_SIZE)

static int findBlocks(int amount);
static void asignMemory(int first_idx, int count, int size);
static void testPrint(int index);

uint8_t start[TOTAL_MEM];
uint8_t* next = start;
int nextindex = 0;

typedef struct block_t {
    uint16_t size;  // = a lo que pidio size = 64 
    uint8_t free;
    int pos;    //pos q tengo en el array
    uint8_t uses_next;
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

    //int count = 0, i, j;
    int first_idx = findBlocks(amount);
    if (first_idx == -1) {
        printStringWC("NO HAY MEMORIA", BLACK, RED);
        printStringLn("");
        return NULL;
    }

    asignMemory(first_idx, amount, size);

    //printStringWC("LLAMADO A MALLOC", BLACK, GREEN);
    //printStringLn("");

    //testPrint();

    return &start[first_idx * BLOCK_SIZE];
}

void free(void * dir) {
    printStringWC("LLAMADO A FREE", BLACK, GREEN);
    printStringLn("");
    int idx = ((uint8_t *) dir - start) / BLOCK_SIZE;
    if (idx < 0 || idx >= TOTAL_BLOCKS) {
        return;
    }
    while (blockArray[idx].uses_next == 1) {
        blockArray[idx].free = 1;
        blockArray[idx].size = 0;
        blockArray[idx].uses_next = 0;
        idx++;
    }
    blockArray[idx].free = 1;
    blockArray[idx].size = 0;
    testPrint(idx);
}

void getMemoryInfo(void) {
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
    printStringWC("INFORMACION DE MEMORIA\n", BLACK, BLUE);
    printString("Memoria total: ");
    printInt(TOTAL_MEM);
    printStringLn(" bytes");
    printString("Memoria en uso: ");
    printInt(blocksUsed * 64);
    printStringLn(" bytes");
    printString("Memoria solicitada: ");
    printInt(usedMemory);
    printStringLn(" bytes");
    printString("Memoria libre: ");
    printInt(freeMemory * 64);
    printStringLn(" bytes");
    printString("Bloques en uso: ");
    printInt(blocksUsed);
    printStringLn("");
}

static void asignMemory(int first_idx, int count, int size) {
    for (int i = first_idx; i < count + first_idx; i++) {
        blockArray[i].size = BLOCK_SIZE;
        blockArray[i].uses_next = 1;
        if (i == count + first_idx - 1) {
            if (size % BLOCK_SIZE != 0)
                blockArray[i].size = size % BLOCK_SIZE;

            blockArray[i].uses_next = 0;
        }
        blockArray[i].free = 0;
        //blockArray[j].pos = j; // Esto lo deberiamos hacer en un init
    }
}

#ifndef BUDDY

static int findBlocks(int amount) {
    int i, count = 0, first_idx;
    for (i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            if (count == 0) {
                first_idx = i;
            }
            count++;
            if (count == amount) {
                return first_idx;
            }
        }
    }
    return -1;
}

#else
// Otro sistema --> menor cantidad de bloques por memoria pedida
static int findBlocks(int amount) {
    printStringLn("BUDDY!");
    int i, count = 0, first_idx, best_fit = INT_MAX, best_fit_idx = -1;
    for (i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            if (count == 0) {
                first_idx = i;
            }
            count++;
        } else {
            if (count >= amount && count < best_fit) {
                best_fit_idx = first_idx;
                best_fit = count;
            }
            count = 0;
        }
    }

    if (count >= amount && count < best_fit) {
        return first_idx;
    }
    
    return best_fit_idx;
}

#endif

static void testPrint(int index) {
    for (int k = 0; k < TOTAL_BLOCKS; k++) {
        if (k != index) {
            printString(" IS FREE: ");
            printInt(blockArray[k].free);
            printString(" USES NEXT: ");
            printInt(blockArray[k].uses_next);
            printString(" SIZE: ");
            printInt(blockArray[k].size);
            printString(" ARRAY POS: ");
            printInt(blockArray[k].pos);
            printString(" DIR: ");
            printInt(start[k * BLOCK_SIZE]);
            printStringLn("");
        }
        else {
            printStringWC(" IS FREE: ",BLACK, RED);
            printInt(blockArray[k].free);
            printStringWC(" USES NEXT: ",BLACK, RED);
            printInt(blockArray[k].uses_next);
            printStringWC(" SIZE: ",BLACK, RED);
            printInt(blockArray[k].size);
            printStringWC(" ARRAY POS: ",BLACK, RED);
            printInt(blockArray[k].pos);
            printStringWC(" DIR: ",BLACK, RED);
            printInt(start[k * BLOCK_SIZE]);
            printStringLn("");
        }
    }
}