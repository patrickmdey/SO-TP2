#include <stddef.h>
#include <syscalls.h>
#include <stringLib.h>

void testShmReader(int argc, char** argv);
void testShmWriter(int argc, char** argv);

void shmTestProcess(int argc, char** argv) {
    sysCreateProcess(&testShmWriter, "shmWriter" , -1, -1, 0, NULL);
    sysCreateProcess(&testShmReader, "shmReader" , -1, -1, 0, NULL);
}

void testShmReader(int argc, char** argv) {
    int64_t shm = sysShmOpen("firstSHM", 0);
    char c = syscall(GETCHAR, shm, 0, 0, 0, 0, 0);
    putchar(c);
    while (1);
}

void testShmWriter(int argc, char** argv) {
    int64_t shm = sysShmOpen("firstSHM", 1);
    sysWrite("Hola", shm);
    while (1);
}