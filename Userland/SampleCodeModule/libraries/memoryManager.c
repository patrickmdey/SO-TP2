#include <memoryManager.h>
#define TOTAL_MEM 1024 * 2
#define BLOCK_SIZE 64

uint8_t start[TOTAL_MEM];
uint8_t* next = start;

uint8_t* dummyMalloc(void) {
    uint8_t* block_start_position = next;
    if (next - start <= TOTAL_MEM - BLOCK_SIZE) {
        next += BLOCK_SIZE;
        return block_start_position;
    }
    else
        return NULL;
}