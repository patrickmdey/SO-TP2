#include <memoryManager.h>
#include <stringLib.h>
#include <systemCalls.h>

void getMemoryInfo(void){
    syscall(GET_MEMORY_INFO, 0, 0, 0, 0, 0, 0);
}

uint8_t* malloc(uint32_t size) {
    return (uint8_t*) syscall(ASIGN_MEMORY, size, 0, 0, 0, 0, 0);
}

void free(uint8_t* dir) {
    syscall(FREE_MEMORY, (uint64_t) dir, 0, 0, 0, 0, 0);
}