#include <taskManager.h>
#include <interrupts.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <staticQueue.h>
#include <lib.h>

#include <list.h>
#include <memoryManager.h>

#include <stringLib.h>
#include <utils.h>

#define SIZE_OF_STACK 4 * 1024

static void* initializeStackFrame(void* entryPoint, void* baseStack);
static t_PCB* getNextProcess();
static t_PCB* getForegroundProcess();
static void fillPs(char** toReturn, int size);

//sacado de stackOverflow
typedef struct {
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



static t_list* tasks;
static t_PCB* current;

static int currentPID = 0;
static int currentTicks = 0;

void* schedule(void* oldRSP, int forceStart) {
      if (forceStart) {
            return current->rsp;
      }

      current->rsp = oldRSP;

      if (currentTicks >= current->priority) { // Change process
            currentTicks = 0;
            current = getNextProcess();
      } else {
            currentTicks++;
      }
      
      return current->rsp;
}

void initTaskManager(void* entryPoint) {
      tasks = malloc(sizeof(t_list));
      if (tasks == NULL)
            return;

      tasks->first = NULL;
      tasks->size = 0;

      createProcess(entryPoint, 0);
      current = tasks->first;
      current->foreground = 1;
}

void createProcess(void* entryPoint, uint8_t background) {
      t_PCB* process = malloc(sizeof(t_PCB));
      if (process == NULL)
            return;

      process->entryPoint = entryPoint;
      process->foreground = 1 - background;
      addProcess(process);
}

int addProcess(t_PCB* process) {
      if (process->entryPoint == 0) {
            return 0;
      }
      process->rbp = malloc(SIZE_OF_STACK);
      if (process->rbp == NULL)
            return -1;

      process->rsp = initializeStackFrame(process->entryPoint, (void*)(process->rbp + SIZE_OF_STACK) - 1);
      process->pid = currentPID++;
      process->state = READY;

      char* arr = (char*)malloc(MAX_SIZE);
      if (arr == NULL)
            return -1;

      t_queue* queue = malloc(sizeof(t_queue));
      if (queue == NULL)
            return -1;

      queue->queue = arr;
      queue->front = 0;
      queue->rear = -1;
      queue->size = 0;
      queue->dim = MAX_SIZE;
      queue->dataSize = sizeof(char);

      process->buffer = queue;
      process->next = NULL;
      insertPCB(tasks, process);
      return 1;
}


void killCurrentProcess() {
      if (tasks->size == 1) {
            return;
      }
      currentTicks = 0;
      free((void*)current->rbp);
      free((void*)current->buffer->queue);
      free((void*)current->buffer);

      removePCB(tasks, current->pid);
}

int killProcess(int pid) {
      if (pid == 0)
            return -1;

      if (pid == current->pid) {
            currentTicks = 0;
      }
      return removePCB(tasks, pid);
}

void resetCurrentProcess() {
      current->rsp = initializeStackFrame(current->entryPoint, (void*)(current->rbp + SIZE_OF_STACK - 1));
      sysForceStart();
}

void writeKeyOnBuffer(char key) {
      t_PCB* foreground = getForegroundProcess();
      queueInsert(foreground->buffer, &key);
}


char removeKeyFromBuffer() {
      char key;
      queueRemoveData(current->buffer, &key);
      return key;
}

char** ps(int* index) {
      int size = tasks->size;
      char** toReturn = malloc((size) * sizeof(char));

      fillPs(toReturn, size);

      *index = size;
      return toReturn;

}

uint8_t changeState(int pid) {
      if (pid == 0)
            return 0;
      t_PCB* pcb = findPCB(tasks, pid);
      if (pcb == NULL)
            return 0;
      if (pcb->state == READY)
            pcb->state = BLOCKED;
      else
            pcb->state = READY;
      return 1;
}

uint8_t changePriority(int pid, int priority) {
      if (pid == 0)
            return 0;
      t_PCB* pcb = findPCB(tasks, pid);
      if (pcb == NULL)
            return 0;

      pcb->priority = priority;
      return 1;
}

int getPID() {
      return current->pid;
}


//guarda el contexto de un proceso
static void* initializeStackFrame(void* entryPoint, void* baseStack) {
      t_stackFrame* frame = (t_stackFrame*)(baseStack - sizeof(t_stackFrame));

      //todos los registros
      frame->r15 = 0x001;
      frame->r14 = 0x002;
      frame->r13 = 0x003;
      frame->r12 = 0x004;
      frame->r11 = 0x005;
      frame->r10 = 0x006;
      frame->r9 = 0x007;
      frame->r8 = 0x008;
      frame->rsi = 0x009;
      frame->rdi = 0x00A;
      frame->rbp = (uint64_t)baseStack; //0x00D
      frame->rdx = 0x00B;
      frame->rcx = 0x00C;
      frame->rbx = 0x00D;
      frame->rax = 0x00E;
      //interrupts StackFrame (es lo que se pushea cuando hay una interrupcion)

      frame->rip = (uint64_t)entryPoint;
      frame->cs = 0x008;
      frame->eflags = 0x202;
      frame->rsp = (uint64_t) & (frame->base);
      frame->ss = 0x000;
      frame->base = 0x000;

      return (void*)(frame);
}

static t_PCB* getNextProcess() {
      int i = 0, size = tasks->size;
      t_PCB* cur = current;

      do {
            if (cur->next != NULL)
                  cur = cur->next;
            else {
                  cur = tasks->first;
            }
            i++;
      } while (i < size && cur->state != READY);

      return cur;
}

static t_PCB* getForegroundProcess() {
      t_PCB* next = current;
      while (next->foreground != 1) {
            if (next->next != NULL) {
                  next = next->next;
            }
            else {
                  next = tasks->first;
            }
      }
      return next;
}

static void fillPs(char** toReturn, int size) {
      int i = 0;
      t_PCB* iterator = tasks->first;
      int offset;

      while (i < size) {
            toReturn[i] = malloc(150);
            offset = 0;
            offset += uintToBase(iterator->pid, toReturn[i] + offset, 10);
            offset += strcpy(toReturn[i] + offset, "     ");
            offset += uintToBase(iterator->priority, toReturn[i] + offset, 10);
            offset += strcpy(toReturn[i] + offset, "          ");
            offset += strcpy(toReturn[i] + offset, iterator->state == READY ? "READY     " : "BLOCKED   ");
            offset += strcpy(toReturn[i] + offset, iterator->foreground == 1 ? "TRUE           " : "FALSE          ");
            offset += uintToBase((uint64_t)iterator->rsp, toReturn[i] + offset, 16);
            offset += strcpy(toReturn[i] + offset, "          ");
            offset += uintToBase((uint64_t)iterator->rbp, toReturn[i] + offset, 16);
            toReturn[i][offset] = 0;

            iterator = iterator->next;
            i++;
      }
}