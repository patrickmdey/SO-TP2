#ifndef PIPE_H
#define PIPE_H

#include <staticQueue.h>
#include <stdint.h>

#define STDIN 0
#define STDOUT 1

typedef struct t_fdNode {
    uint64_t fd;
    t_queue* buffer;
    struct t_fdNode* next;
} t_fdNode;

typedef struct  t_fdList{
    t_fdNode * first;
    uint64_t size;
} t_fdList;

void initPipes();
void insertFd(t_fdNode* fdNode);
uint8_t removeFd(uint64_t fd);
t_fdNode * findFd(uint64_t fd);
uint64_t getFdSize();

void pipeWrite(t_fdNode * node, char c);
void pipeWriteStr(t_fdNode * node, char * str);

#endif