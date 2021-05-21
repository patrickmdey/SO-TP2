#ifndef WAITING_PID_H
#define WAITING_PID_H

typedef struct t_waitingPid {
    uint64_t pid;
    struct t_waitingPid * next;
} t_waitingPid;

#endif