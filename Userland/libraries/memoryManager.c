#include <memoryManager.h>
#include <stringLib.h>
#include <syscalls.h>

uint8_t* malloc(uint32_t size) {
    return (uint8_t*)syscall(ASIGN_MEMORY, size, 0, 0, 0, 0, 0);
}

void free(void * dir) {
    syscall(FREE_MEMORY, (uint64_t)dir, 0, 0, 0, 0, 0);
}