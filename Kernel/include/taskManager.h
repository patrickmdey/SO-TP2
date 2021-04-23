#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define MAX_PROCESSES 2

#define READY 1
#define CPU 2
#define BLOCKED 3


#include <stdint.h>
#include <buffer.h>
#include <staticQueue.h>

typedef struct t_PCB{
    void * entryPoint;      //Puntero a la funcion que debe ejecutarse
    void * rsp;             //StackFrame
    void * rbp;             //Base Pointer
    int stackID;            //El stack que le corresponde (4KB)
    //t_bufferID bufferID;    //El buffer que le corresponde --> CAMBIAR
    t_queue * buffer;

    struct t_PCB * next;
    uint8_t state;
    int priority;
    int pid;
} t_PCB;     //Process Control Block

extern t_queue taskManager;

void initTaskManager(void * entryPoint);

void* schedule(void* oldRSP, int forceStart);
int addProcess(t_PCB* process);
void killCurrentProcess();
void resetCurrentProcess();

void writeKeyOnBuffer(char key);
char removeKeyFromBuffer();

#endif