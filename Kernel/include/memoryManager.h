#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h> 

void initMemory();

uint8_t * myMalloc(uint32_t size);

void myFree(uint8_t* dir);

void getMemoryInfo(void);


#endif