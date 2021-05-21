#include <taskManager.h>
#include <interrupts.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <staticQueue.h>
#include <lib.h>
#include <pipe.h>

#include <list.h>
#include <memoryManager.h>

#include <utils.h>

#define SIZE_OF_STACK 4 * 1024

static void* initializeStackFrame(void* entryPoint, void* baseStack, int argc, char** argv);
static t_PCB* getNextProcess();
static int fillPs(char** toReturn, int size);

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
static t_PCB* current = NULL;

static int currentPID = 0;
static int currentTicks = 0;

void* schedule(void* oldRSP, int forceStart) {
      if (forceStart) {
            return current->rsp;
      }

      current->rsp = oldRSP;

      if (current->state != READY || currentTicks >= current->priority) { // Change process
            t_PCB* aux = current;
            uint8_t state = aux->state;
            currentTicks = 0;
            current = getNextProcess();
            /*printString("pid:");
            printInt(current->pid);*/
            if (state == YIELD)
                  aux->state = READY;

      } else {
            currentTicks++;
      }

      return current->rsp;
}

void initTaskManager(void * entryPoint) {
      tasks = malloc(sizeof(t_list));
      if (tasks == NULL)
            return;

      tasks->first = NULL;
      tasks->size = 0;

      createProcess(entryPoint, "shell", -1, -1, 0, 0);
      current = tasks->first;
      current->foreground = 1;
}

uint64_t getCurrentPid() {
      return current->pid;
}

int64_t createProcess(void * entryPoint, char * name, int64_t fdIn, int64_t fdOut, uint8_t argc, char ** argv) {
      t_PCB* process = malloc(sizeof(t_PCB));
      if (process == NULL)
            return -1;

      uint8_t background = (argc > 0 && argv[argc - 1][0] == '&');
      process->entryPoint = entryPoint;
      process->name = name;
      process->foreground = 1 - background;
      process->argv = argv;
      process->argc = argc - background;

      if (current == NULL) {
            process->in = STDIN;
            process->out = STDOUT;
      } else {
            process->in = (background) ? -1 : ( (fdIn == -1) ? current->in : fdIn ); // si esta en background seteo a -1, 
                  // sino chequeo si me pasaron un fd y seteo a el, sino seteo al del padre
            process->out = (fdOut == -1) ? current->out : fdOut;
      }
      /*printInt(process->in);
      printInt(process->out);*/

      addProcess(process);
      return process->pid;
}

int addProcess(t_PCB* process) {
      if (process->entryPoint == 0) {
            return 0;
      }
      process->rbp = malloc(SIZE_OF_STACK);
      if (process->rbp == NULL)
            return -1;

      process->rsp = initializeStackFrame(process->entryPoint, (void*)(process->rbp + SIZE_OF_STACK) - 1,
            process->argc, process->argv);
      process->pid = currentPID++;
      process->state = READY;

      //process->buffer = queueInit(sizeof(char));

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
      /*free((void*)current->buffer->queue);
      free((void*)current->buffer);*/

      removePCB(tasks, current->pid);
}

int killProcess(int pid) {
      if (pid == 0)
            return -1;

      if (pid == current->pid) {
            currentTicks = 0;
      }

      t_PCB* pcb = findPCB(tasks, pid);
      if (pcb == NULL)
            return 0;

      pcb->state = KILLED;

      return 1;
}

void exit() {
      killProcess(current->pid);
      int_20();
}

void resetCurrentProcess() {
      current->rsp = initializeStackFrame(current->entryPoint, (void*)(current->rbp + SIZE_OF_STACK - 1),
            current->argc, current->argv);
      sysForceStart();
}

void writeKeyOnBuffer(char key) { // INPUT DE TECLADO
      //t_PCB* foreground = getForegroundProcess();
      t_fdNode * in = findFd(STDIN);
      //queueInsert(in->buffer, &key);
      pipeWrite(in, key);
}

char removeKeyFromBuffer() { // getchar
      char key = 0;
      t_fdNode * in = findFd(current->in);
      if (in != NULL) {
            queueRemoveData(in->buffer, &key);
      }
      return key;
}

char** ps(int* index) {
      int size = tasks->size;
      char** toReturn = malloc((size) * sizeof(char*));

      *index = fillPs(toReturn, size);

      return toReturn;

}

void yield() {
      current->state = YIELD;
      int_20();
}

uint8_t block(int pid) {

      t_PCB* pcb = findPCB(tasks, pid);
      if (pcb == NULL)
            return 0;
      
      if (pcb->state == KILLED)
            return 0;
      
      if (pcb->state == READY)
            pcb->state = BLOCKED;
      else if (pcb->state == BLOCKED)
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

uint8_t changeForeground(int pid) {
      t_PCB* pcb = findPCB(tasks, pid);
      if (pcb == NULL)
            return 0;
      if (pcb->foreground == 1)
            pcb->foreground = 0;
      else if (pcb->foreground == 0)
            pcb->foreground = 1;
      return 1;
}

int getPID() {
      return current->pid;
}

uint64_t getCurrentOut() {
      return current->out;
}


//guarda el contexto de un proceso
static void* initializeStackFrame(void* entryPoint, void* baseStack, int argc, char** argv) {
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
      frame->rsi = (uint64_t)argv;
      frame->rdi = argc;
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
      t_PCB* curr = current;
      do {
            t_PCB* aux = curr->next;
            if (curr->state == KILLED) {
                  removePCB(tasks, curr->pid);
            }
            if (aux != NULL)
                  curr = aux;
            else {
                  curr = tasks->first;
            }
            i++;
      } while (i < size && curr->state != READY);

      return curr;
}

static int fillPs(char** toReturn, int size) {
      int i = 0, j = 0;
      t_PCB* iterator = tasks->first;
      int offset;

      while (i < size) {
            if (iterator->state != KILLED) {
                  toReturn[j] = malloc(150);
                  offset = 0;
                  offset += uintToBase(iterator->pid, toReturn[j] + offset, 10);
                  offset += strcpy(toReturn[j] + offset, "     ");
                  offset += uintToBase(iterator->priority, toReturn[j] + offset, 10);
                  offset += strcpy(toReturn[j] + offset, "          ");
                  offset += strcpy(toReturn[j] + offset, iterator->state == READY ? "READY     " : "BLOCKED   ");
                  offset += strcpy(toReturn[j] + offset, iterator->foreground == 1 ? "TRUE           " : "FALSE          ");
                  offset += uintToBase((uint64_t)iterator->rsp, toReturn[j] + offset, 16);
                  offset += strcpy(toReturn[j] + offset, "          ");
                  offset += uintToBase((uint64_t)iterator->rbp, toReturn[j] + offset, 16);
                  offset += strcpy(toReturn[j] + offset, "     ");
                  offset += strcpy(toReturn[j] + offset, iterator->name);
                  toReturn[j][offset] = 0;
                  j++;
            }
            iterator = iterator->next;
            i++;
      }
      return j;
}
