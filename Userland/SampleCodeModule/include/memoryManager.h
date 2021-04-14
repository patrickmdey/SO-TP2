#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h> 

void initMemory();

uint8_t * dummyMalloc(uint32_t size);

void free(uint8_t* dir);


#endif