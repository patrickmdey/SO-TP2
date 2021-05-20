#include <sem.h>
#include <stringLib.h>
#include <utils.h>
#include <syscalls.h>
#include <stddef.h>
#include <phylo.h>
#include <memoryManager.h>

#define MAX_PHYLOSOPHERS 15
#define VERY_BIG_NUMBER 999999

t_sem* forks[MAX_PHYLOSOPHERS] = { NULL };
t_sem* printSem;

int size = 5;

char eating[MAX_PHYLOSOPHERS] = { 0 };

static void printProcess(int argc, char** argv) {
    while (1) {
        semWait(printSem);
        printStringLn(eating);
        semPost(printSem);
        //sysYield();
    }
}


static void phyloProcess(int argc, char** args) {
    if (argc <= 0 || argc > 4)
        printStringLn("Too many arguments");
    int error = 0;
    int id = strToInt(args[0], &error);
    //int size = strToInt(args[1], &error);
    //size = 5;
    int leftFork, rightFork;

    //{0, 0, 0 ,0, 0, 0}

    //{0, 1, 2, 3, 4}     size = 5
    // {0, 1, 2, 3, 4}   size = 5

    while (1) {
        /*printInt(id);
        printStringLn(" waiting");*/
        rightFork = id;
        if (id == 0)
            leftFork = size - 1;
        else
            leftFork = id - 1;

        if (id % 2 == 0) {
            semWait(forks[rightFork]);
            semWait(forks[leftFork]);

            /*printInt(id);
            printStringLn(" eating");*/

            semWait(printSem);
            eating[id] = args[0][0];
            semPost(printSem);


            for (int i = 0; i < VERY_BIG_NUMBER; i++);

            semWait(printSem);
            eating[id] = '.';
            semPost(printSem);

            /*printInt(id);
            printStringLn(" done");*/

            semPost(forks[leftFork]);
            semPost(forks[rightFork]);

        } else {
            semWait(forks[leftFork]);
            semWait(forks[rightFork]);

            /*printInt(id);
            printStringLn(" eating");*/
            semWait(printSem);
            eating[id] = args[0][0];
            semPost(printSem);

            for (int i = 0; i < VERY_BIG_NUMBER; i++);

            semWait(printSem);
            eating[id] = '.';
            semPost(printSem);

            /*printInt(id);
            printStringLn(" done");*/

            semPost(forks[rightFork]);
            semPost(forks[leftFork]);
        }
    }
}

void phylo(int argc, char** args) {
    sysBlock(0);
    int64_t pids[MAX_PHYLOSOPHERS] = { 0 };
    int64_t printPid = 0;
    size = 5;

    printSem = semOpen("printSem", 1, 1);

    int i;
    char** names = (char**)malloc(10 * sizeof(char*));
    for (i = 0; i < size;i++) {
        names[i] = (char*)malloc(3 * sizeof(char));
        uintToBase(i, names[i], 10);
        forks[i] = semOpen(names[i], 1, 1);
    }

    char* params[5][2] = {
          { "0", "&" },
          { "1", "&" },
          { "2", "&" },
          { "3", "&" },
          { "4", "&" }
    };

    for (i = 0; i < size; i++) {
        pids[i] = sysCreateProcess(&phyloProcess, "eater", -1, -1, 2, params[i]);
        eating[i] = '.';
        sysBlock(pids[i]);
    }
    eating[i] = 0;
    for (i = 0; i < size; i++)
        sysBlock(pids[i]);

    printPid = sysCreateProcess(&printProcess, "printer", -1, -1, 0, NULL);

    char c;
    while (1) {
        c = getchar();
        if (c == 'a') {
            char** param = (char**)malloc(2 * sizeof(char*));
            param[0] = (char*)malloc(3 * sizeof(char));
            uintToBase(size, param[0], 10);
            param[1] = "&";
            forks[size] = semOpen(param[0], 1, 1);
            eating[size] = '.';
            pids[size++] = sysCreateProcess(&phyloProcess, "eater", -1, -1, 2, param);
        } else if (c == 'r') {
            ;
            // printStringLn("WAITING");
            // if ((size - 1) % 2 != 0) {
            //     semWait(forks[size - 1]); // espero liberar fork izq
            //     semWait(forks[0]);
            // } else {
            //     semWait(forks[0]);
            //     semWait(forks[size - 1]); // espero liberar fork izq
            // }
            // printStringLn("KILLING");
            // sysKill(pids[size - 1]);
            // size--;
            // eating[size] = 0;
            // semPost(forks[0]);
            // semPost(forks[size]);
            // semClose(forks[size]);
            // forks[size] = NULL;
            // printStringLn("DONE");
        } else if (c == '\t') {
            for (i = 0; i < size; i++) {
                sysKill(pids[i]);
                semClose(forks[i]);
                free(names[i]);
            }

            semClose(printSem);

            sysKill(printPid);
            free(names);
            sysBlock(0);
            sysExit();
        }
    }
}