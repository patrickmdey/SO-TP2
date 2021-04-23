#include <taskManager.h>
#include <interrupts.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <staticQueue.h>
#include <lib.h>

#include <list.h>
#include <memoryManager.h>

#include <stringLib.h>

#define SIZE_OF_STACK 4 * 1024

static void* initializeStackFrame(void* entryPoint, void* baseStack);

//sacado de stackOverflow
typedef struct {
      uint64_t gs;
      uint64_t fs;
      uint64_t r15;
      uint64_t r14;
      uint64_t r13;
      uint64_t r12;
      uint64_t r11;
      uint64_t r10;
      uint64_t r9;
      uint64_t r8;
      uint64_t rsi;
      uint64_t rdi;
      uint64_t rbp;
      uint64_t rdx;
      uint64_t rcx;
      uint64_t rbx;
      uint64_t rax;

      uint64_t rip;
      uint64_t cs;
      uint64_t eflags;
      uint64_t rsp;
      uint64_t ss;
      uint64_t base;
} t_stackFrame;



t_list tasks = { 0, NULL, NULL };
t_PCB* current;

int currentPID = 0;


void initTaskManager(void* entryPoint) {
      t_PCB* shell = (t_PCB*)malloc(sizeof(t_PCB));
      if (shell == NULL)
            return;

      shell->entryPoint = entryPoint;

      addProcess(shell);
      current = shell;
}

void* schedule(void* oldRSP, int forceStart) {
      if (forceStart) {
            return current->rsp;
      }
      t_PCB* nextProcess;
      nextProcess = current->next;
      if (nextProcess == NULL) {
            nextProcess = tasks.first;
      }
      //pcb1 ---> pcb2 ---> pcb3

      
      current->rsp = oldRSP; //OBLIGATORIO
      current = nextProcess;
      //}

      return nextProcess->rsp;
}

int addProcess(t_PCB* process) {
      if (process->entryPoint == 0) {
            return 0;
      }
      process->rbp = malloc(SIZE_OF_STACK);
      if (process->rbp == NULL)
            return -1;

      process->rsp = initializeStackFrame(process->entryPoint, (void*)(process->rbp + SIZE_OF_STACK - 1));
      process->pid = currentPID++;
      process->state = READY;

      char* arr = (char*)malloc(MAX_SIZE);
      if (arr == NULL)
            return -1;

      t_queue* queue = (t_queue*)malloc(sizeof(t_queue));
      if (queue == NULL)
            return -1;

      queue->queue = arr;
      queue->front = 0;
      queue->rear = -1;
      queue->size = 0;
      queue->dim = MAX_SIZE;
      queue->dataSize = sizeof(char);

      process->buffer = queue;
      //creo Buffer
      insertPCB(&tasks, process);
      return 1;
}


void killCurrentProcess() {
      if (tasks.size == 1) {
            return;
      }
      free((void*)current->rbp);
      free((void*)current->buffer->queue);
      free((void*)current->buffer);

      removePCB(&tasks, current->pid);
}

void resetCurrentProcess() {
      if (tasks.size == 1) {
            return;
      }

      current->rsp = initializeStackFrame(current->entryPoint, (void*)(current->rbp + SIZE_OF_STACK - 1));
      //queueUpdateFirst(&taskManager, &currentProcess);
      sys_forceStart();
}

void writeKeyOnBuffer(char key) {
      queueInsert(current->buffer, &key);
}


char removeKeyFromBuffer() {
      char key;
      queueRemoveData(current->buffer, &key);
      return key;
}


//sacado de stackOverflow
//guarda el contexto de un proceso
static void* initializeStackFrame(void* entryPoint, void* baseStack) {
      t_stackFrame* frame = (t_stackFrame*)baseStack - 1;
      //todos los registros
      frame->gs = 0x001;
      frame->fs = 0x002;
      frame->r15 = 0x003;
      frame->r14 = 0x004;
      frame->r13 = 0x005;
      frame->r12 = 0x006;
      frame->r11 = 0x007;
      frame->r10 = 0x008;
      frame->r9 = 0x009;
      frame->r8 = 0x00A;
      frame->rsi = 0x00B;
      frame->rdi = 0x00C;
      frame->rbp = (uint64_t) &(frame->base); //0x00D
      frame->rdx = 0x00E;
      frame->rcx = 0x00F;
      frame->rbx = 0x010;
      frame->rax = 0x011;
      //interrupts StackFrame (es lo que se pushea cuando hay una interrupcion)

      frame->rip = (uint64_t)entryPoint;             //LO MAS IMPORTANTE
      frame->cs = 0x008;
      frame->eflags = 0x202;
      frame->rsp = (uint64_t) &(frame->base);
      frame->ss = 0x000;
      frame->base = 0x000;

      return (void*)(frame);
}