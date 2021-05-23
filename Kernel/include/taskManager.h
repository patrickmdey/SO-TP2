#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define MAX_PROCESSES 2

#define READY 1
#define BLOCKED 2
#define YIELD 3
#define KILLED 4

#include <stdint.h>
#include <buffer.h>
#include <staticQueue.h>
#include <waitingPid.h>

typedef struct t_PCB {
    void* entryPoint;
    void* rsp;
    void* rbp;

    struct t_PCB* next;
    uint8_t state;
    uint8_t foreground;
    int priority;
    uint64_t pid;
    char* name;

    t_waitingPid * waiting;

    int64_t in;
    int64_t out;

    int argc;
    char ** argv;
} t_PCB;     //Process Control Block

extern t_queue taskManager;

void initTaskManager(void* entryPoint);

void* schedule(void* oldRSP, int forceStart);
uint64_t getCurrentPid();
int64_t createProcess(void* entryPoint, char* name, int64_t fdIn, int64_t fdOut, uint8_t argc, char ** argv);
int addProcess(t_PCB* process);
void killCurrentProcess();
void resetCurrentProcess();
int killProcess(int pid);

void exit();

void waitpid(uint64_t pid);

int getPID();
char** ps(int* index);
uint8_t block(int pid);
void yield();

uint64_t getCurrentOut();

void writeKeyOnBuffer(char key);
char removeKeyFromBuffer();

uint8_t changePriority(int pid, int priority);
uint8_t changeForeground(int pid);

#endif