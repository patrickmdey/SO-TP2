#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define MAX_PROCESSES 2

#define READY 1
#define BLOCKED 2
#define KILLED 3

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
    char *name;

    uint64_t arg1;
    uint64_t arg2;
    uint64_t arg3;
} t_PCB;     //Process Control Block

extern t_queue taskManager;

void initTaskManager(void* entryPoint);

void* schedule(void* oldRSP, int forceStart);
uint64_t getCurrentPid();
void createProcess(void* entryPoint, char* name, uint8_t background, uint64_t arg1, uint64_t arg2, uint64_t arg3);
int addProcess(t_PCB* process);
void killCurrentProcess();
void resetCurrentProcess();
int killProcess(int pid);

void exit();

int getPID();
char** ps(int* index);
uint8_t changeState(int pid);

void writeKeyOnBuffer(char key);
char removeKeyFromBuffer();

uint8_t changePriority(int pid, int priority);

#endif