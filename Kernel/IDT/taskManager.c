// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <interrupts.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <staticQueue.h>
#include <lib.h>
#include <pipe.h>

#include <pcbQueue.h>
#include <memoryManager.h>

#include <utils.h>

#define SIZE_OF_STACK (8 * 1024)

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



static t_pcbQueue* tasks;
static t_PCB* current = NULL;

static uint64_t currentPID = 0;
static int currentTicks = 0;
static int idleId;
static uint8_t isIdle = 0;

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
            if (state == YIELD)
                  aux->state = READY;

      }
      else {
            currentTicks++;
      }

      return current->rsp;
}

void initTaskManager(void* entryPoint) {
      tasks = createPcbQueue();
      if (tasks == NULL)
            return;

      tasks->first = NULL;
      tasks->size = 0;

      createProcess(entryPoint, "shell", -1, -1, 0, 0);
      idleId = createProcess(&idleProcess, "idle", -1, -1, 0, NULL);
      block(idleId);
      current = tasks->first;
      current->foreground = 1;
}

uint64_t getCurrentPid() {
      return current->pid;
}

int64_t createProcess(void* entryPoint, char* name, int64_t fdIn, int64_t fdOut, uint8_t argc, char** argv) {
      if (entryPoint == 0)
            return -1;
      t_PCB* process = malloc(sizeof(t_PCB));
      if (process == NULL)
            return -1;

      uint8_t background = (argc > 0 && argv[argc - 1][0] == '&');
      process->entryPoint = entryPoint;
      process->name = name;
      process->foreground = 1 - background;
      process->argv = argv;
      process->argc = argc - background;

      //process->parent = current;
      process->pid = currentPID++;


      if (current == NULL) {
            process->in = STDIN;
            process->out = STDOUT;
      }
      else {
            process->in = (background) ? -1 : ((fdIn == -1) ? current->in : fdIn); // si esta en background seteo a -1, 
                  // sino chequeo si me pasaron un fd y seteo a el, sino seteo al del padre
            process->out = (fdOut == -1) ? current->out : fdOut;
            /*t_waitingPid * child = malloc(sizeof(t_waitingPid));
            child->pid = process->pid;
            child->next = NULL;
            if (current->children == NULL) {
                  current->children = child;
            } else {
                  t_waitingPid * curr = current->children;
                  while (curr->next != NULL) {
                        curr = curr->next;
                  }
                  curr->next = child;
            }*/
      }

      process->waiting = NULL;
      process->priority = 0;

      //process->children = NULL;

      /*process->addresses = malloc(sizeof(t_addressList));
      if (process->addresses == NULL) {
            free(process);
            return -1;
      }
      process->addresses->first = NULL;
      process->addresses->size = 0;*/


      process->state = READY;
      process->rbp = malloc(SIZE_OF_STACK);
      if (process->rbp == NULL) {
            free(process);
            return -1;
      }

      process->rsp = initializeStackFrame(process->entryPoint, (void*)(process->rbp + SIZE_OF_STACK) - 1,
            process->argc, process->argv);


      /*printInt(process->in);
      printInt(process->out);*/

      int ret = addProcess(process);

      if (ret != 1) {
            //freeAddressList(process->addresses);
            free(process->rbp);
            free(process);
            return -1;
      }
      return process->pid;
}

int addProcess(t_PCB* process) {

      //process->buffer = queueInit(sizeof(char));

      process->next = NULL;
      insertPCB(tasks, process);
      return 1;
}


int killProcess(int pid) {
      if (pid == 0 || pid == idleId)
            return -1;

      if (pid == current->pid) {
            currentTicks = 0;
      }


      t_PCB* pcb = findPCB(tasks, pid);
      if (pcb == NULL || pcb->state == KILLED)
            return 0;

      t_waitingPid* curr = pcb->waiting;
      while (curr != NULL) {
            block(curr->pid);
            curr = curr->next;
      }

      pcb->state = KILLED;

      return 1;
}

void exit() {
      killProcess(current->pid);
      int_20();
}

void waitpid(uint64_t pid) {
      t_PCB* pcb = findPCB(tasks, pid);
      if (pcb == NULL || pcb->state == KILLED)
            return;


      t_waitingPid* toAdd = malloc(sizeof(t_waitingPid));
      if (toAdd == NULL)
            return;

      toAdd->pid = current->pid;
      toAdd->next = NULL;
      t_waitingPid* curr = pcb->waiting;
      if (curr == NULL)
            pcb->waiting = toAdd;
      else {
            while (curr->next != NULL)
                  curr = curr->next;

            curr->next = toAdd;
      }
      block(current->pid);
      int_20();

}

void resetCurrentProcess() {
      current->rsp = initializeStackFrame(current->entryPoint, (void*)(current->rbp + SIZE_OF_STACK - 1),
            current->argc, current->argv);
      sysForceStart();
}

void writeKeyOnBuffer(char key) { // INPUT DE TECLADO
      t_fdNode* in = findFd(STDIN);
      pipeWrite(in, key);
}

char getChar(int64_t fd) { // getchar
      //char key = 0;
      uint64_t readFd;
      if (fd == -1)
            readFd = current->in;
      else
            readFd = fd;

      /*t_fdNode * in = findFd(readFd);
      if (in != NULL) {
            queueRemoveData(in->buffer, &key);
            if (key == 0)

      }
      return key;*/

      return pipeRead(readFd);
}

void* allocateMem(uint32_t size) {
      void* dir = malloc(size);
      /*if (dir == NULL)
            return NULL;

      int ret = addAddress(current->addresses, dir);
      if (ret == 0) {
            free(dir);
            return NULL;
      }*/
      return dir;
}

void freeMem(void* dir) {
      //removeAddress(current->addresses, dir);
      free(dir);
}

char** ps(int* index) {
      int size = tasks->size;
      char** toReturn = malloc((size) * sizeof(char*));

      *index = fillPs(toReturn, size);

      if (*index == 0) {
            return NULL;
      }

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

void idleProcess() {
      while (1);
}

int64_t shmOpen(char * name, uint8_t create) {
      int64_t fd = getShmFd(name, create);
      if (fd == -1 || current->fdsIdx == MAX_FDS)
            return -1;

      current->fds[(current->fdsIdx)++] = fd;
      return fd;
}

void shmClose(int64_t fd) {
      uint8_t removed = closeShmFd(fd);
      if (removed == 0)
            return;

      int i, j = 0, found = 0;
      int64_t * fdArr = current->fds;
      for (i = 0; i < current->fdsIdx; i++) {
            if (fdArr[i] != fd) {
                  fdArr[j++] = fdArr[i];
                  found = 1;
            }
      }
      if (found)
            (current->fdsIdx)--;
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
      int i = 0;
      t_PCB* curr;
      do {
            curr = peekFirst(tasks);
            if (curr->state == KILLED) {
                  removePCB(tasks, curr->pid);
            }
            else {
                  curr = popFirst(tasks);
                  insertPCB(tasks, curr);
            }
            i++;
      } while (i < getSize(tasks) && curr->state != READY);

      if (!isIdle && curr->state != READY) {
            isIdle = 1; // desbloqueo idle process
            block(idleId);
            return findPCB(tasks, idleId); // run idle process;
      }
      else if (isIdle && curr->state == READY) {
            isIdle = 0;
            block(idleId); // block idle process
      }

      return curr;
}

static int fillPs(char** toReturn, int size) {
      int i = 0, j = 0;
      t_PCB* iterator = tasks->first;
      int offset;

      while (i < size) {
            if (iterator->state != KILLED) {
                  toReturn[j] = malloc(150);
                  if (toReturn[j] == NULL) {
                        int k;
                        for (k = 0; k < j; k++) {
                              free(toReturn[k]);
                        }
                        free(toReturn);
                        return 0;
                  }
                  offset = 0;
                  offset += uintToBase(iterator->pid, toReturn[j] + offset, 10);
                  offset += strcpy(toReturn[j] + offset, "     ");
                  offset += uintToBase(iterator->priority, toReturn[j] + offset, 10);
                  offset += strcpy(toReturn[j] + offset, "          ");
                  offset += strcpy(toReturn[j] + offset, iterator->state == READY ? "READY     " : "BLOCKED   ");
                  offset += strcpy(toReturn[j] + offset, iterator->foreground == 1 ? "TRUE           0x" : "FALSE          0x");
                  offset += uintToBase((uint64_t)iterator->rsp, toReturn[j] + offset, 16);
                  offset += strcpy(toReturn[j] + offset, "        0x");
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
