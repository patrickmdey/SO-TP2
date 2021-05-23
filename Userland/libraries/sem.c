#include <sem.h>
#include <syscalls.h>

t_sem * semOpen(char *name, uint8_t create, uint64_t value){
    return (t_sem *) syscall(SEM_OPEN, (uint64_t) name, (uint64_t) create, value, 0, 0, 0);
}

void semInit(t_sem * sem, int value) {
    syscall(SEM_INIT, (uint64_t) sem, (uint64_t) value, 0, 0, 0, 0);
}

void semWait(t_sem * sem) {
    syscall(SEM_WAIT, (uint64_t) sem, 0, 0, 0, 0, 0);
}

void semPost(t_sem * sem) {
    syscall(SEM_POST, (uint64_t) sem, 0, 0, 0, 0, 0);
}

void semClose(t_sem * sem) {
    syscall(SEM_CLOSE, (uint64_t) sem, 0, 0, 0, 0, 0);
}

void semDestroy(t_sem * sem) {
    syscall(SEM_DESTROY, (uint64_t) sem, 0, 0, 0, 0, 0);
}