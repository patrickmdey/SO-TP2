#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define MAX_PROCESSES 2

#define READY 1
#define BLOCKED 2


#include <stdint.h>
#include <buffer.h>
#include <staticQueue.h>

typedef struct t_PCB {
    void* entryPoint;
    void* rsp;
    void* rbp;
    t_queue* buffer;

    struct t_PCB* next;
    uint8_t state;
    uint8_t foreground;
    int priority;
    int pid;
} t_PCB;     //Process Control Block

extern t_queue taskManager;

void initTaskManager(void* entryPoint);

void* schedule(void* oldRSP, int forceStart);
void createProcess(void* entryPoint, uint8_t background);
int addProcess(t_PCB* process);
void killCurrentProcess();
void resetCurrentProcess();
int killProcess(int pid);

int getPID();
char** ps(int* index);
uint8_t changeState(int pid);

void writeKeyOnBuffer(char key);
char removeKeyFromBuffer();

uint8_t changePriority(int pid, int priority);

#endif