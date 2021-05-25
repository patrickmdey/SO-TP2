// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stringLib.h>
#include <syscalls.h>
#include <test_util.h>
#include <commands.h>

#define MAX_PROCESSES 10 //Should be around 80% of the the processes handled by the kernel

enum State { ERROR, RUNNING, BLOCKED, KILLED };

typedef struct P_rq {
    int64_t pid;
    enum State state;
}p_rq;

void endless_loop() {
    while (1);
}

void test_processes() {
    p_rq p_rqs[MAX_PROCESSES];
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;

    char* params[] = { "&" };

    while (1) {


        // Create MAX_PROCESSES processes
        for (rq = 0; rq < MAX_PROCESSES; rq++) {
            p_rqs[rq].pid = sysCreateProcess(&endless_loop, "endless_loop", -1, -1, 1, params);  // TODO: Port this call as required
            // printInt( p_rqs[rq].pid);
            // printString(" ");

            if (p_rqs[rq].pid == -1) {                           // TODO: Port this as required
                printString("Error creating process\n");               // TODO: Port this as required
                sysExit();
            }
            else {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {

            for (rq = 0; rq < MAX_PROCESSES; rq++) {
                action = GetUniform(2) % 2;
                // printString("ACTION: ");
                // printInt(action);
                // printStringLn("");

                switch (action) {
                case 0:
                    if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
                        if (sysKill(p_rqs[rq].pid) != 1) {          // TODO: Port this as required
                            printString("Error killing process\n");        // TODO: Port this as required
                            sysExit();
                        }
                        p_rqs[rq].state = KILLED;
                        alive--;
                    }
                    break;

                case 1:
                    if (p_rqs[rq].state == RUNNING) {
                        if (sysBlock(p_rqs[rq].pid) == 0) {          // TODO: Port this as required
                            printString("Error blocking process\n");       // TODO: Port this as required
                            sysExit();
                        }
                        p_rqs[rq].state = BLOCKED;
                    }
                    break;
                }
            }

            sysYield();

            // Randomly unblocks processes
            for (rq = 0; rq < MAX_PROCESSES; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2) {
                    if (sysBlock(p_rqs[rq].pid) == 0) {            // TODO: Port this as required
                        printString("Error unblocking process\n");         // TODO: Port this as required
                        sysExit();
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }
    }
    sysExit();
}