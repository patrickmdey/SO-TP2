#include <stdint.h>
#include <syscalls.h>
#include <stringLib.h>

#define TOTAL_PROCESSES 3
#define MINOR_WAIT 1000000
#define WAIT      10000000 

static void bussyWait(uint64_t n) {
    uint64_t i;
    for (i = 0; i < n; i++);
}

static void endlessLoop() {
    uint64_t pid = sysGetPid();

    while (1) {
        printInt(pid);
        bussyWait(MINOR_WAIT);
    }
}

void test_prio() {
    uint64_t pids[TOTAL_PROCESSES];
    uint64_t i;

    char* params[] = { "&" };

    for (i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = sysCreateProcess(&endlessLoop, "endlessLoop", -1, -1, 1, params);

    bussyWait(WAIT);
    printString("\nCHANGING PRIORITIES...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++) {
        switch (i % 3) {
        case 0:
            sysNice(pids[i], 0); //lowest priority 
            break;
        case 1:
            sysNice(pids[i], 1); //medium priority
            break;
        case 2:
            sysNice(pids[i], 2); //highest priority
            break;
        }
    }

    bussyWait(WAIT);
    printString("\nBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sysBlock(pids[i]);

    printString("CHANGING PRIORITIES WHILE BLOCKED...\n");
    for (i = 0; i < TOTAL_PROCESSES; i++) {
        switch (i % 3) {
        case 0:
            sysNice(pids[i], 1); //medium priority 
            break;
        case 1:
            sysNice(pids[i], 1); //medium priority
            break;
        case 2:
            sysNice(pids[i], 1); //medium priority
            break;
        }
    }

    printString("UNBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sysBlock(pids[i]);

    bussyWait(WAIT);
    printString("\nKILLING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sysKill(pids[i]);

    sysExit();
}