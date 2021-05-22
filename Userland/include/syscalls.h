#ifndef SYSCALLS
#define SYSCALLS

//dataTypes
#include <RTCTime.h>
#include <taskManager.h>
#include <colours.h>
#include <cpuInfo.h>
#include <stdint.h>

typedef enum {
    GET_MEM = 0,
    RTC_TIME = 1,
    TEMP = 2,
    WRITE = 3,
    GETCHAR = 4,
    CLEAR = 5,
    LOAD_APP = 6,
    RUN = 7,
    EXIT = 8,
    INFOREG = 9,
    DRAW = 10,
    MOVE_CURSOR = 11,
    MOVE_CURSOR_TO = 12,
    CURSOR_POSITION = 13,
    GET_TICKS_ELAPSED = 14,
    ASIGN_MEMORY = 15,
    FREE_MEMORY = 16,
    GET_MEMORY_INFO = 17,
    PS = 18,
    CREATE_PROCESS = 19,
    GET_PID = 20,
    KILL = 21,
    NICE = 22,
    BLOCK = 23,
    FOREGROUND = 24,
    SEM_OPEN = 25,
    SEM_INIT = 26,
    SEM_WAIT = 27,
    SEM_POST = 28,
    SEM_CLOSE = 29,
    YIELD = 30,
    SEM_INFO = 31,
    GET_FD = 32,
    WAIT_PID = 33,
    SEM_DESTROY = 34
} syscallID;

uint64_t syscall(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9);
void sys_changeApp();

int sysGetTicksElapsed();

void sysExit();

void sysClear(int fromWidth, int fromHeight, int toWidth, int toHeight);

uint8_t sysRTCTime(uint8_t type);

void sysGetMem(uint64_t memDir, uint8_t* memData);

int sysTemp();

char **sysGetMeminfo(int *size);

char **sysPs(int *size);

int sysGetPid();

int sysKill(int pid);

void sysWaitpid(uint64_t pid);

char **sysSemInfo(int *size);

int sysNice(int pid, int priority);

uint8_t sysBlock(int pid);

uint8_t sysChangeForeground(int pid);

uint64_t *sysInfoReg();

int64_t sysCreateProcess(void * entryPoint, char* name, int64_t fdInt, int64_t fdOut, uint8_t argc, char** args);

void sysYield(void);

int64_t sysGetFd();

void sysDraw(char* bitmap, t_colour colour, int multiplier);

void sysMoveCursor(int x, int y);

void sysMoveCursorTo(int x, int y);

void sysCursorPosition(int* array);
#endif