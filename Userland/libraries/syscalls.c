#include <syscalls.h>

int sysGetTicksElapsed() {
    return syscall(GET_TICKS_ELAPSED, 0, 0, 0, 0, 0, 0);
}

void sysExit() {
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
    return (char**)syscall(GET_MEMORY_INFO, (uint64_t)&size, 0, 0, 0, 0, 0);
}

char** sysPs(int * size) {
    return (char**)syscall(PS, (uint64_t) size, 0, 0, 0, 0, 0);
}

char **sysSemInfo(int *size){
    return (char**)syscall(SEM_INFO, (uint64_t)&size, 0, 0, 0, 0, 0);
}

int sysGetPid(){
    return syscall(GET_PID, 0, 0, 0, 0, 0, 0);
}

int sysKill(int pid){
    return syscall(KILL, pid, 0, 0, 0, 0, 0);
}

int sysNice(int pid, int priority){
    return syscall(NICE, pid, priority, 0, 0, 0, 0);
}

int sysBlock(int pid){
    return syscall(BLOCK, pid, 0, 0, 0, 0, 0);
}

uint64_t *sysInfoReg(){
    return (uint64_t*)syscall(INFOREG, 0, 0, 0, 0, 0, 0);
}

void sysCreateProcess(void * entryPoint, char* name, int argc, char** args){
      uint8_t background = argc > 0 && args[argc - 1][0] == '&';

      // for(int i = 0; i<argc; i++)
      //       printStringLn(args[i]);
      
      syscall(CREATE_PROCESS, (uint64_t) entryPoint, (uint64_t) name, background, 
                  (uint64_t) args[0], (uint64_t) args[1], (uint64_t) args[2]);
}

void sysYield(void){
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