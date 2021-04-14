#include <memoryManager.h>
#include <stringLib.h>

#define TOTAL_MEM 1024 * 2
#define BLOCK_SIZE 64
#define TOTAL_BLOCKS (TOTAL_MEM/BLOCK_SIZE) - 15

static int findBlocks(int amount);
static void asignMemory(int first_idx, int count, int size);
static void testPrint(void);

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

// {f,u,f,f}

// 64: 100 : 

void initMemory() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        blockArray[i].free = 1;
        blockArray[i].pos = i;
    }
}

uint8_t* dummyMalloc(uint32_t size) {
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

    printStringWC("LLAMADO A MALLOC", BLACK, GREEN);
    printStringLn("");

    for (int k = 0; k < TOTAL_BLOCKS; k++) {
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

    return &start[first_idx * BLOCK_SIZE];
}

 

void free(uint8_t* dir) {
    printStringWC("LLAMADO A FREE", BLACK, GREEN);
    printStringLn("");    
    int idx = (dir - start) / BLOCK_SIZE;
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
    testPrint();
}


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

static void testPrint(void){
    for (int k = 0; k < TOTAL_BLOCKS; k++) {
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
}