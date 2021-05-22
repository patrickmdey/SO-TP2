#include <syscalls.h>
#include <shell.h>
#include <utils.h>

int sysGetTicksElapsed() {
    return syscall(GET_TICKS_ELAPSED, 0, 0, 0, 0, 0, 0);
}

void sysExit() {
    printPrompt();
    syscall(EXIT, 0, 0, 0, 0, 0, 0);

}

void sysClear(int fromWidth, int fromHeight, int toWidth, int toHeight) {
    syscall(CLEAR, fromWidth, fromHeight, toWidth, toHeight, 0, 0);
}

uint8_t sysRTCTime(uint8_t type) {
    return syscall(RTC_TIME, type, 0, 0, 0, 0, 0);
}

void sysGetMem(uint64_t memDir, uint8_t* memData) {
    syscall(GET_MEM, memDir, (uint64_t)memData, 0, 0, 0, 0);
}

int sysTemp() {
    return syscall(TEMP, 0, 0, 0, 0, 0, 0);
}

char** sysGetMeminfo(int* size) {
    return (char**)syscall(GET_MEMORY_INFO, (uint64_t)size, 0, 0, 0, 0, 0);
}

char** sysPs(int* size) {
    return (char**)syscall(PS, (uint64_t)size, 0, 0, 0, 0, 0);
}

char** sysSemInfo(int* size) {
    return (char**)syscall(SEM_INFO, (uint64_t)size, 0, 0, 0, 0, 0);
}

int sysGetPid() {
    return syscall(GET_PID, 0, 0, 0, 0, 0, 0);
}

int sysKill(int pid) {
    return syscall(KILL, pid, 0, 0, 0, 0, 0);
}

int64_t sysGetFd() {
    return (int64_t)syscall(GET_FD, 0, 0, 0, 0, 0, 0);
}

int sysNice(int pid, int priority) {
    return syscall(NICE, pid, priority, 0, 0, 0, 0);
}

uint8_t sysBlock(int pid) {
    return syscall(BLOCK, pid, 0, 0, 0, 0, 0);
}

uint8_t sysChangeForeground(int pid) {
    return syscall(FOREGROUND, pid, 0, 0, 0, 0, 0);
}

uint64_t* sysInfoReg() {
    return (uint64_t*)syscall(INFOREG, 0, 0, 0, 0, 0, 0);
}

int64_t sysCreateProcess(void* entryPoint, char* name, int64_t fdIn, int64_t fdOut, uint8_t argc, char** argv) {
    return syscall(CREATE_PROCESS, (uint64_t)entryPoint, (uint64_t)name, fdIn, fdOut, argc, (uint64_t)argv);
}

void sysYield(void) {
    syscall(YIELD, 0, 0, 0, 0, 0, 0);
}

//dibuja bitmap
void sysDraw(char* bitmap, t_colour colour, int multiplier) {
    syscall(DRAW, (uint64_t)bitmap, colour, multiplier, 0, 0, 0);
}

void sysMoveCursor(int x, int y) {
    syscall(MOVE_CURSOR, x, y, 0, 0, 0, 0);
}

void sysMoveCursorTo(int x, int y) {
    syscall(MOVE_CURSOR_TO, x, y, 0, 0, 0, 0);
}

void sysCursorPosition(int* array) {
    syscall(CURSOR_POSITION, (uint64_t)array, 0, 0, 0, 0, 0);
}