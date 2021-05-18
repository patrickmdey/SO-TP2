#include <test_sync.h>
#include <utils.h>
#include <stringLib.h>
#include <sem.h>
#include <commands.h>
#include <syscalls.h>

#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID "sem"

int64_t global;

void test_sync() {
    uint64_t i;

    global = 0;

    printStringLn("CREATING PROCESSES...(WITH SEM)");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        char* array1[4] = { "1", "1", "100", "&" };
        char* array2[4] = { "1", "-1", "100", "&" };
        sysCreateProcess(&inc, "inc", 4, array1);
        sysCreateProcess(&inc, "dec", 4, array2);
    }

    ps(0, 0, 0);
    while(1);
    sysExit();
}

void test_no_sync() {
    uint64_t i;

    global = 0;

    printStringLn("CREATING PROCESSES...(WITHOUT SEM)");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        char* array1[4] = { "0", "1", "100", "&" };
        char* array2[4] = { "0", "-1", "100", "&" };
        sysCreateProcess(&inc, "inc", 4, array1);
        sysCreateProcess(&inc, "dec", 4, array2);
    }
    while(1);
    sysExit();
}

void inc(char* semC, char* valueC, char* NC) {
    int error = 0;
    int sem = strToInt(semC, &error);
    if (error) {
        printStringLn("Errors parsing sem");
        sysExit();
    }
    int64_t value = strToInt(valueC, &error);
    if (error) {
        printStringLn("Errors parsing value");
        sysExit();
    }

    int64_t N = strToInt(NC, &error);
    if (error) {
        printStringLn("Errors parsing N");
        sysExit();
    }

    int64_t i;
    t_sem * semp;
    if (sem) {
        semp = semOpen(SEM_ID, 1, 1);
        if (!semp) {
            printStringLn("ERROR OPENING SEM");
            sysExit();
        }
    }

    for (i = 0; i < N; i++) {
        if (sem) {
            semWait(semp);
        }

        slowInc(&global, value);

        if (sem) {
            semPost(semp);
        }
    }

    if (sem) {
        semClose(semp);
        printStringLn("CLOSED SEM");
    }

    printString("Final value: ");
    printInt(global);
    printStringLn(" ");
    printStringLn("FINISHED");
    sysExit();
    printStringLn("Flasheo");
}

void slowInc(int64_t* p, int64_t inc) {
    int64_t aux = *p;
    aux += inc;
    sysYield();
    *p = aux;
}
