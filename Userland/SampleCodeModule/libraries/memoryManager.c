#include <memoryManager.h>
#include <stringLib.h>
#include <systemCalls.h>

void getMemoryInfo(void){
    syscall(GET_MEMORY_INFO, 0, 0, 0, 0, 0, 0);
}

void initMemory() {
    syscall(INIT_MEM, 0, 0, 0, 0, 0, 0);
}

uint8_t* allocMemory(uint32_t size) {
    return (uint8_t*) syscall(ASIGN_MEMORY, size, 0, 0, 0, 0, 0);
}

void freeMemory(uint8_t* dir) {
    syscall(FREE_MEMORY, (uint64_t) dir, 0, 0, 0, 0, 0);
}