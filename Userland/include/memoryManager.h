#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h> 

uint8_t * malloc(uint32_t size);

void free(void* dir);


#endif