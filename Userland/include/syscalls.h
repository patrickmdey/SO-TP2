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
    ASIGN_MEMORY = 14,
    FREE_MEMORY = 15,
    GET_MEMORY_INFO = 16,
    PS = 17,
    CREATE_PROCESS = 18,
    GET_PID = 19,
    KILL = 20,
    NICE = 21,
    BLOCK = 22,
    FOREGROUND = 23,
    SEM_OPEN = 24,
    SEM_INIT = 25,
    SEM_WAIT = 26,
    SEM_POST = 27,
    SEM_CLOSE = 28,
    YIELD = 29,
    SEM_INFO = 30,
    GET_FD = 31,
    WAIT_PID = 32,
    SEM_DESTROY = 33,
    CLOSE_FD = 34,
    PIPE_INFO = 35
} syscallID;

uint64_t syscall(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9);
void sys_changeApp();

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

char **sysPipeInfo(int *size);

int sysNice(int pid, int priority);

uint8_t sysBlock(int pid);

uint8_t sysChangeForeground(int pid);

uint64_t *sysInfoReg();

int64_t sysCreateProcess(void * entryPoint, char* name, int64_t fdInt, int64_t fdOut, uint8_t argc, char** args);

void sysYield(void);

int64_t sysGetFd();

void sysCloseFd(int64_t fd);

void sysDraw(char* bitmap, t_colour colour, int multiplier);

void sysMoveCursor(int x, int y);

void sysMoveCursorTo(int x, int y);

void sysCursorPosition(int* array);
#endif