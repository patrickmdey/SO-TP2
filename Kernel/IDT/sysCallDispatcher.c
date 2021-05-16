#include <sysCallDispatcher.h>
#include <RTCTime.h>
#include <stringLib.h>
#include <lib.h>
#include <interrupts.h>
#include <taskManager.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <timerTick.h>
#include <memoryManager.h>
#include <sem.h>

#define SYS_GETMEM_ID 0
#define SYS_RTC_TIME_ID 1
#define SYS_TEMP_ID 2
#define SYS_WRITE_ID 3
#define SYS_GETCHAR_ID 4
#define SYS_CLEAR_ID 5
#define SYS_LOAD_APP_ID 6
#define SYS_RUN_ID 7
#define SYS_EXIT_ID 8
#define SYS_INFOREG_ID 9
#define SYS_DRAW 10
#define SYS_MOVE_CURSOR 11
#define SYS_MOVE_CURSOR_TO 12
#define SYS_CURRENT_CURSOR 13
#define SYS_TICKS_ELAPSED 14
#define SYS_ASIGN_MEMORY 15
#define SYS_FREE_MEMORY 16
#define SYS_GET_MEM_INFO 17
#define SYS_PS 18
#define SYS_CREATE_PROCESS 19
#define SYS_GET_PID 20
#define SYS_KILL 21
#define SYS_NICE 22
#define SYS_BLOCK 23
#define SYS_SEM_OPEN 24
#define SYS_SEM_INIT 25
#define SYS_SEM_WAIT 26
#define SYS_SEM_POST 27
#define SYS_SEM_CLOSE 28
#define SYS_HALT 29

#define SYSCALLS 29

uint64_t sysCallDispatcher(t_registers* r) {
      if (r->rax >= 0 && r->rax <= SYSCALLS)
      {
            switch (r->rax)
            {
            case SYS_GETMEM_ID:
                  sysGetMem(r->rdi, (uint8_t*)r->rsi);
                  break;

            case SYS_RTC_TIME_ID:
                  return getDecimalTimeInfo((t_timeInfo)(r->rdi));
                  break;

            case SYS_TEMP_ID:
                  return cpuTemp();
                  break;

            case SYS_WRITE_ID:
                  sysWrite((char*)(r->rdi), (uint8_t)(r->rsi), (t_colour)(r->rdx), (t_colour)(r->r10));
                  break;

            case SYS_GETCHAR_ID:
                  if ((int)(r->rdi) == 1)
                        return getcharOnce();
                  return getchar();
                  break;

            case SYS_CLEAR_ID:
                  if ((int)(r->rdi) == 0 && (int)(r->rsi) == 0 && (int)(r->rdx) == 0 && (int)(r->r10) == 0)
                        clearScreen();
                  else
                        clearScreenFromTo((int)(r->rdi), (int)(r->rsi), (int)(r->rdx), (int)(r->r10));
                  break;

            case SYS_LOAD_APP_ID:
                  return addProcess((t_PCB*)r->rdi);
                  break;

            case SYS_RUN_ID:
                  sysForceStart();
                  break;

            case SYS_EXIT_ID:
                  killCurrentProcess();
                  break;

            case SYS_INFOREG_ID:
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
            case SYS_TICKS_ELAPSED:
                  return ticksElapsed();
                  break;
            case SYS_ASIGN_MEMORY:
                  return (uint64_t)malloc((uint8_t)r->rdi);
                  break;
            case SYS_FREE_MEMORY:
                  free((uint8_t*)r->rdi);
                  break;
            case SYS_GET_MEM_INFO:
                  return (uint64_t)getMemoryInfo((uint64_t *) r->rdi);
                  break;
            case SYS_PS:
                  return (uint64_t)ps((int*)((uint64_t)(r->rdi)));
                  break;
            case SYS_CREATE_PROCESS:
                  createProcess((void*)r->rdi, (char *) r->rsi , (uint8_t) r->rdx, r->r10, r->r8, r->r9);
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
                  return (uint64_t)changeState((int)r->rdi);
                  break;
            case SYS_SEM_OPEN:
                  return (uint64_t) semOpen((char*) r->rdi, (uint8_t) r->rsi, (uint64_t) r->rdx);
                  break;
            case SYS_SEM_INIT:
                  semInit((void *) r->rdi, (int) r->rsi);
                  break;
            case SYS_SEM_WAIT:
                  semWait((void *)r->rdi);
                  break;
            case SYS_SEM_POST:
                  semPost((void *) r->rdi);
                  break;
            case SYS_SEM_CLOSE:
                  semClose((void *)r->rdi);
                  break;
            case SYS_HALT:
                  _hlt();
                  break;
            }
      }
      return 0;
}
