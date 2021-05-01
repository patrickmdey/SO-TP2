#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h> 

void initMemory();

void* malloc(uint32_t size);

void free(void* dir);

void getMemoryInfo(void);


#endif