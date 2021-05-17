#include <test_sync.h>
#include <utils.h>
#include <stringLib.h>
#include <sem.h>
#include <commands.h>
#include <systemCalls.h>

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
        createProcess(&inc, "inc", 4, array1);
        createProcess(&inc, "dec", 4, array2);
    }
    while (1);
}

void test_no_sync() {
    uint64_t i;

    global = 0;

    printStringLn("CREATING PROCESSES...(WITHOUT SEM)");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        char* array1[4] = { "0", "1", "100", "&" };
        char* array2[4] = { "0", "-1", "100", "&" };
        createProcess(&inc, "inc", 4, array1);
        createProcess(&inc, "inc", 4, array2);
    }

    while (1);
}

void inc(char* semC, char* valueC, char* NC) {
    int error = 0;
    int sem = strToInt(semC, &error);
    if (error) {
        printStringLn("Errors parsing sem");
        // syscall exit
        return;
    }
    int64_t value = strToInt(valueC, &error);
    if (error) {
        printStringLn("Errors parsing value");
        return;

    }
    int64_t N = strToInt(NC, &error);
    if (error) {
        printStringLn("Errors parsing N");
        return;
    }

    int64_t i;
    t_sem * semp;
    if (sem) {
        semp = semOpen(SEM_ID, 1, 1);
        printStringLn("OPENED SEM");
        if (!semp) {
            printStringLn("ERROR OPENING SEM");
            return;
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
        //semClose(semp);
        printStringLn("CLOSED SEM");
    }

    printString("Final value: ");
    printInt(global);
    printStringLn(" ");
    printStringLn("FINISHED");
    /*int pid = syscall(GET_PID, 0, 0, 0, 0, 0, 0);
    syscall(KILL, pid, 0, 0, 0, 0, 0);*/
    while (1);
    printStringLn("Flasheo");
}

void slowInc(int64_t* p, int64_t inc) {
    int64_t aux = *p;
    aux += inc;
    yield();
    *p = aux;
}
