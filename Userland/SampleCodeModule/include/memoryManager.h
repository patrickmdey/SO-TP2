#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h> 

void initMemory();

uint8_t * allocMemory(uint32_t size);

void freeMemory(uint8_t* dir);

void getMemoryInfo(void);


#endif