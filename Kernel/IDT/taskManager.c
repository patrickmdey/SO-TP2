#include <taskManager.h>
#include <interrupts.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <staticQueue.h>
#include <lib.h>

#define SIZE_OF_STACK 4 * 1024

static void* getNewStackBase();
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

static t_PCB processes[MAX_PROCESSES];

static uint8_t stacks[MAX_PROCESSES][SIZE_OF_STACK], freeStacks[MAX_PROCESSES]={0};

t_queue taskManager = {processes, 0, -1, 0, MAX_PROCESSES, sizeof(t_PCB)};

void* schedule(void* oldRSP, int forceStart) {
      t_PCB currentProcess, nextProcess;

      if (!forceStart) {
            queueRemoveData(&taskManager, &currentProcess);
            currentProcess.rsp = oldRSP;
            queueInsert(&taskManager, &currentProcess);
      }

      queuePeek(&taskManager, &nextProcess);

      changeBuffer(nextProcess.bufferID);

      return nextProcess.rsp;
}

int addProcess(t_PCB* process) {
      if( process->entryPoint==0 || queueIsFull(&taskManager)){
            return 0;
      }
      process->rbp = getNewStackBase();
      process->rsp = initializeStackFrame(process->entryPoint, (void*)(process->rbp + SIZE_OF_STACK - 1));

      queueInsert(&taskManager, process);
      return 1;
}

void killCurrentProcess() {
      if(queueIsEmpty(&taskManager)){
            return;
      }
      t_PCB currentProcess;
      queueRemoveData(&taskManager, &currentProcess);
      freeStacks[currentProcess.stackID] = 0;
      addProcess(&currentProcess);
      sys_forceStart();
}

void resetCurrentProcess(){
      if (queueIsEmpty(&taskManager)) {
            return;
      }
      t_PCB currentProcess;
      queuePeek(&taskManager,&currentProcess);
      currentProcess.rsp = initializeStackFrame(currentProcess.entryPoint, (void*)(currentProcess.rbp + SIZE_OF_STACK - 1));
      queueUpdateFirst(&taskManager,&currentProcess);
      sys_forceStart();
}

static void* getNewStackBase() {
      for (int i = 0; i < MAX_PROCESSES; i++)
      {
            if(freeStacks[i]==0){
                  freeStacks[i]=1;
                  return stacks[i];
            }
      }
      return 0;
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
      frame->rbp = 0x00D;
      frame->rdx = 0x00E;
      frame->rcx = 0x00F;
      frame->rbx = 0x010;
      frame->rax = 0x011;
      //interrupts StackFrame (es lo que se pushea cuando hay una interrupcion)
      
      frame->rip = (uint64_t)entryPoint;             //LO MAS IMPORTANTE
      frame->cs = 0x008;
      frame->eflags = 0x202;
      frame->rsp = (uint64_t) & (frame->base);
      frame->ss = 0x000;
      frame->base = 0x000;

      return (void*)(frame);
}