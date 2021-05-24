#include <sysCallDispatcher.h>
#include <RTCTime.h>
#include <stringLib.h>
#include <lib.h>
#include <interrupts.h>
#include <taskManager.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <memoryManager.h>
#include <sem.h>
#include <pipe.h>

#define SYS_GETMEM 0
#define SYS_RTC_TIME 1
#define SYS_TEMP 2
#define SYS_WRITE 3
#define SYS_GETCHAR 4
#define SYS_CLEAR 5
#define SYS_LOAD_APP 6
#define SYS_RUN 7
#define SYS_EXIT 8
#define SYS_INFOREG 9
#define SYS_DRAW 10
#define SYS_MOVE_CURSOR 11
#define SYS_MOVE_CURSOR_TO 12
#define SYS_CURRENT_CURSOR 13
#define SYS_ASIGN_MEMORY 14
#define SYS_FREE_MEMORY 15
#define SYS_GET_MEM_INFO 16
#define SYS_PS 17
#define SYS_CREATE_PROCESS 18
#define SYS_GET_PID 19
#define SYS_KILL 20
#define SYS_NICE 21
#define SYS_BLOCK 22
#define SYS_FOREGROUND 23
#define SYS_SEM_OPEN 24
#define SYS_SEM_INIT 25
#define SYS_SEM_WAIT 26
#define SYS_SEM_POST 27
#define SYS_SEM_CLOSE 28
#define SYS_YIELD 29
#define SYS_SEM_INFO 30
#define SYS_GET_FD 31
#define SYS_WAIT_PID 32
#define SYS_SEM_DESTROY 33
#define SYS_CLOSE_FD 34
#define SYS_PIPE_INFO 35

#define SYSCALLS 35

uint64_t sysCallDispatcher(t_registers* r) {
      if (r->rax >= 0 && r->rax <= SYSCALLS)
      {
            switch (r->rax)
            {
            case SYS_GETMEM:
                  sysGetMem(r->rdi, (uint8_t*)r->rsi);
                  break;

            case SYS_RTC_TIME:
                  return getDecimalTimeInfo((t_timeInfo)(r->rdi));
                  break;

            case SYS_TEMP:
                  return cpuTemp();
                  break;

            case SYS_WRITE:
                  sysWrite((int64_t) r->rdi, (char*) (r->rsi), (uint8_t) (r->rdx), (t_colour)(r->r10), (t_colour) (r->r8));
                  break;

            case SYS_GETCHAR:
                  /*if ((int)(r->rdi) == 1)
                        return getcharOnce();*/

                  return getChar((int64_t) r->rdi);
                  break;

            case SYS_CLEAR:
                  if ((int)(r->rdi) == 0 && (int)(r->rsi) == 0 && (int)(r->rdx) == 0 && (int)(r->r10) == 0)
                        clearScreen();
                  else
                        clearScreenFromTo((int)(r->rdi), (int)(r->rsi), (int)(r->rdx), (int)(r->r10));
                  break;

            case SYS_LOAD_APP:
                  return addProcess((t_PCB*)r->rdi);
                  break;

            case SYS_RUN:
                  sysForceStart();
                  break;

            case SYS_EXIT:
                  exit();
                  break;

            case SYS_INFOREG:
                  return (uint64_t)getSnapshot();
                  break;
            case SYS_DRAW:
                  draw((char*)(r->rdi), (t_colour)(r->rsi), (uint32_t)(r->rdx));
                  break;
            case SYS_MOVE_CURSOR:
                  moveCursor((uint32_t)r->rdi, (uint32_t)r->rsi);
                  break;
            case SYS_MOVE_CURSOR_TO:
                  moveCursorTo((uint32_t)r->rdi, (uint32_t)r->rsi);
                  break;
            case SYS_CURRENT_CURSOR:
                  cursorPosition((int*)((uint64_t)(r->rdi)));
                  break;
            case SYS_ASIGN_MEMORY:
                  return (uint64_t) allocateMem((uint32_t) r->rdi);
                  break;
            case SYS_FREE_MEMORY:
                  freeMem((void *) r->rdi);
                  break;
            case SYS_GET_MEM_INFO:
                  return (uint64_t)getMemoryInfo((uint64_t*)r->rdi);
                  break;
            case SYS_PS:
                  return (uint64_t)ps((int*)((uint64_t)(r->rdi)));
                  break;
            case SYS_CREATE_PROCESS:
                  return createProcess((void*)r->rdi, (char*)r->rsi, (int64_t)r->rdx, (int64_t)r->r10, r->r8, (char**)r->r9);
                  break;
            case SYS_GET_PID:
                  return getPID();
                  break;
            case SYS_KILL:
                  return killProcess((int)r->rdi);
                  break;
            case SYS_NICE:
                  return changePriority((int)r->rdi, (int)r->rsi);
                  break;
            case SYS_BLOCK:
                  return (uint64_t)block((int)r->rdi);
                  break;
            case SYS_FOREGROUND:
                  return (uint64_t)changeForeground((int)r->rdi);
                  break;
            case SYS_SEM_OPEN:
                  return (uint64_t)semOpen((char*)r->rdi, (uint8_t)r->rsi, (uint64_t)r->rdx);
                  break;
            case SYS_SEM_INIT:
                  semInit((void*)r->rdi, (int)r->rsi);
                  break;
            case SYS_SEM_WAIT:
                  semWait((void*)r->rdi);
                  break;
            case SYS_SEM_POST:
                  semPost((void*)r->rdi);
                  break;
            case SYS_SEM_CLOSE:
                  semClose((void*)r->rdi);
                  break;
            case SYS_YIELD:
                  yield();
                  break;
            case SYS_SEM_INFO:
                  return (uint64_t)semInfo((int*)r->rdi);
                  break;
            case SYS_GET_FD:
                  return (uint64_t)getFd();
                  break;
            case SYS_WAIT_PID:
                  waitpid((uint64_t)r->rdi);
                  break;
            case SYS_SEM_DESTROY:
                  semDestroy((void*)r->rdi);
                  break;
            case SYS_CLOSE_FD:
                  closeFd((int64_t) r->rdi);
                  break;
            case SYS_PIPE_INFO:
                  return (uint64_t) pipeInfo((int*) ((uint64_t)(r->rdi)));
                  break;
            }

      }
      return 0;
}
