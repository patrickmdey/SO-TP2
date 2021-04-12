#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define MAX_PROCESSES 2

#include <stdint.h>
#include <buffer.h>
#include <staticQueue.h>

typedef struct{
    void * entryPoint;      //Puntero a la funcion que debe ejecutarse
    void * rsp;             //StackFrame
    void* rbp;              //Base Pointer
    int stackID;            //El stack que le corresponde (4KB)
    t_bufferID bufferID;    //El buffer que le corresponde
}t_PCB;     //Process Control Block

extern t_queue taskManager;

void* schedule(void* oldRSP, int forceStart);
int addProcess(t_PCB* process);
void killCurrentProcess();
void resetCurrentProcess();

#endif