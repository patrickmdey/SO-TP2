#include <sem.h>
#include <stringLib.h>
#include <utils.h>
#include <syscalls.h>
#include <stddef.h>
#include <phylo.h>
#include <memoryManager.h>

#define MAX_PHYLOSOPHERS 15
#define VERY_BIG_NUMBER 999999
#define INTIAL_PHYLOSOHERS 5

static void addEater();
static void removeEater();
static void eat(int id);

t_sem * forks[MAX_PHYLOSOPHERS] = {NULL};
int64_t pids[MAX_PHYLOSOPHERS] = {0};
t_sem * printSem;
t_sem * creatingSem;
t_sem * changeMutex;

int size;

char eating[MAX_PHYLOSOPHERS] = {0};

int sig[MAX_PHYLOSOPHERS] = {0};

static void printProcess(int argc, char **argv) {
    semWait(creatingSem);
    int counter;
    while (1) {
        semWait(printSem);
        counter = 0;
        for (int i = 0; i < size; i++) {
            if (eating[i] == '.')
                counter++;
            else {
                break;
            }
        }
        if (counter != size)
            printStringLn(eating);

        semPost(printSem);
    }
}

static void phyloProcess(int argc, char **args) {
    if (argc <= 0 || argc > 1)
        printStringLn("Too many arguments");

    int error = 0;
    int id = strToInt(args[0], &error);
    if (error) {
        printStringLn("Invalid parameter id");
        sysExit();
    }

    int leftFork, rightFork;

    semWait(creatingSem);

    //{0, 0, 0 ,0, 0, 0}

    //{0, 1, 2, 3, 4}     size = 5
    // {0, 1, 2, 3, 4}   size = 5

    //{0, 1, 2, 3, 4, 5}     size = 6
    // {0, 1, 2, 3, 4, 5}   size = 6

    while (1) {

        rightFork = id;
        if (id == 0)
            leftFork = size - 1;
        else
            leftFork = id - 1;

        if (id % 2 == 0) {
            semWait(forks[rightFork]);
            semWait(forks[leftFork]);

            if (sig[id] != 0) {
                if (id == 0) {
                    if (sig[id] == 1) {
                        semPost(forks[leftFork]);
                        semWait(forks[size - 1]);
                    }

                    leftFork = size - 1;
                    sig[0] = 0;
                    semPost(changeMutex);
                } else if (sig[id] == -1) {
                    removeEater();
                } else {
                    addEater();
                    semPost(creatingSem);
                }
            }

            eat(id);

            semPost(forks[leftFork]);
            semPost(forks[rightFork]);
        } else {
            semWait(forks[leftFork]);
            semWait(forks[rightFork]);

            if (sig[id] == -1) {
                removeEater();
            } else if (sig[id] == 1) {
                //sig[id] = 0;
                addEater();
                semPost(creatingSem);
            }

            eat(id);

            semPost(forks[rightFork]);
            semPost(forks[leftFork]);
        }
    }
}

void phylo(int argc, char **args) {
    int64_t printPid;

    size = 0;

    creatingSem = semOpen("creatingSem", 1, 0);
    printSem = semOpen("printSem", 1, 1);
    changeMutex = semOpen("changeMutex", 1, 1);

    int i;
    for (i = 0; i < INTIAL_PHYLOSOHERS; i++) {
        addEater(pids);
    }

    char ** printParams = (char **) malloc(sizeof(char *));
    printParams[0] = (char *) malloc(sizeof(char));
    printParams[0][0] = '&';

    printPid = sysCreateProcess(&printProcess, "printer", -1, -1, 1, printParams);

    for (i = 0; i < size + 1; i++) {
        semPost(creatingSem);
    }

    char c;
    while (1) {
        c = getchar();
        if (c == 'a')
            if (size > 14)
                printStringWC("Too many phylosophers\n", BLACK, RED);
            else {
                semWait(changeMutex);
                sig[size - 1] = 1;
            }
        else if (c == 'r') {
            if (size - 1 > 2) {
                semWait(changeMutex);
                sig[size - 1] = -1;
            } else
                printStringWC("Too few phylosophers\n", BLACK, RED);
        } else if (c == '\t'){
            for (i = 0; i < size; i++) {
                semClose(forks[i]);
                sysKill(pids[i]);
            }
            semClose(changeMutex);
            semClose(creatingSem);
            semClose(printSem);
            sysKill(printPid);
            sysExit();
        }
    }
}

static void addEater(){
    printStringWC("Agrego filosofo\n", BLACK, RED);
    char ** param = (char **) malloc(2 * sizeof(char *));
    param[0] = (char *) malloc(3 * sizeof(char));
    uintToBase(size, param[0], 10);
    param[1] = (char *) malloc(1 * sizeof(char));
    param[1][0] = '&';
    sig[size] = 0;
    if (size != 0)
        sig[size - 1] = 0;

    eating[size++] = '.';
    eating[size] = 0;
    sig[0] = 1;
    forks[size - 1] = semOpen(param[0], 1, 1);
    pids[size - 1] = sysCreateProcess(&phyloProcess, "eater", -1, -1, 2, param);
}

static void removeEater() {
    semWait(printSem);
    eating[size - 1] = 0;
    printStringWC("Mato filosofo\n", BLACK, RED);
    semPost(printSem);
    size--;
    sig[0] = -1;
    sig[size] = 0;
    semPost(forks[size]);
    semClose(forks[size]);
    sysExit();
}

static void eat(int id) {
    semWait(printSem);
    eating[id] = 'E';
    semPost(printSem);
    for (int i = 0; i < VERY_BIG_NUMBER; ++i);
    semWait(printSem);
    eating[id] = '.';
    semPost(printSem);
}