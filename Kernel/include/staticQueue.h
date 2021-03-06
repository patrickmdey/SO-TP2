#ifndef STATIC_QUEUE
#define STATIC_QUEUE

#include <stdint.h>

#define MAX_SIZE 150

typedef struct {
      void* queue;
      int front;
      int rear;
      int size;
      int dim;
      uint64_t dataSize;
} t_queue;

t_queue * queueInit(uint64_t dataSize);
void queuePeek(t_queue* queue, void* data);
void queueUpdateFirst(t_queue* queue, void* data);
int queueIsEmpty(t_queue* queue);
int queueIsFull(t_queue* queue);
int queueSize(t_queue* queue);
void queueInsert(t_queue* queue, void* data);
void queueRemoveData(t_queue* queue, void* data);
void freeQueue(t_queue * queue);

#endif