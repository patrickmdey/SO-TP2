// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <test_sync.h>
#include <utils.h>
#include <stringLib.h>
#include <sem.h>
#include <commands.h>
#include <syscalls.h>

#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID "sem"

int64_t global;

void test_sync(int argc, char** argv) {
    uint64_t i;

    uint64_t pids[TOTAL_PAIR_PROCESSES * 2];

    global = 0;

    printStringLn("CREATING PROCESSES...(WITH SEM)");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        char* array1[4] = { "1", "1", "100", "&" };
        char* array2[4] = { "1", "-1", "100", "&" };
        pids[i] = sysCreateProcess(&inc, "inc", -1, -1, 4, array1);
        pids[i + 1] = sysCreateProcess(&inc, "dec", -1, -1, 4, array2);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES * 2; i++)
        sysWaitpid(pids[i]);

    sysExit();
}

void test_no_sync(int argc, char** argv) {
    uint64_t i;

    global = 0;

    uint64_t pids[TOTAL_PAIR_PROCESSES * 2];

    printStringLn("CREATING PROCESSES...(WITHOUT SEM)");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        char* array1[4] = { "0", "1", "100", "&" };
        char* array2[4] = { "0", "-1", "100", "&" };
        pids[i] = sysCreateProcess(&inc, "inc", -1, -1, 4, array1);
        pids[i + 1] = sysCreateProcess(&inc, "dec", -1, -1, 4, array2);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES * 2; i++)
        sysWaitpid(pids[i]);

    sysExit();
}

void inc(int argc, char** argv) {
    if (argc < 3) {
        printStringLn("Not enough arguments");
        sysExit();
    }
    int error = 0;
    int sem = strToInt(argv[0], &error);
    if (error) {
        printStringLn("Errors parsing sem");
        sysExit();
    }
    int64_t value = strToInt(argv[1], &error);
    if (error) {
        printStringLn("Errors parsing value");
        sysExit();
    }
    int64_t N = strToInt(argv[2], &error);
    if (error) {
        printStringLn("Errors parsing N");
        sysExit();
    }

    int64_t i;
    t_sem* semp;
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
    }

    printString("Final value: ");
    printInt(global);
    printStringLn(" ");
    printStringLn("FINISHED");
    sysExit();
}

void slowInc(int64_t* p, int64_t inc) {
    int64_t aux = *p;
    aux += inc;
    sysYield();
    *p = aux;
}
