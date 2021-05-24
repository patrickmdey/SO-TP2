#include <syscalls.h>
#include <commands.h>
#include <stringLib.h>
#include <tests.h>

static void fakeSleep();
static int createLoop();
static void printPs();
static void printMemoryInfo();
char* params[] = { "&" };

void testContextSwitching() {

    printStringWC("You can add, block, kill or change priority of any loop while this test is running\n", BLACK, GREEN);
    printMemoryInfo();
    fakeSleep();

    int pid1 = createLoop();
    fakeSleep();

    int pid2 = createLoop();
    fakeSleep();

    int pid3 = createLoop();
    fakeSleep();
    printPs();

    printStringWC("Changing loop with pid ", BLACK, GREEN);
    printInt(pid2);
    printStringWC(" to priority 4\n", BLACK, GREEN);
    sysNice(pid2, 4);
    fakeSleep();
    printPs();

    printStringWC("Blocking loop with pid ", BLACK, GREEN);
    printInt(pid1);
    printStringLn("");
    sysBlock(pid1);
    fakeSleep();
    printPs();

    int pid4 = createLoop();
    fakeSleep();

    printStringWC("Blocking all test loops\n", BLACK, GREEN);
    sysBlock(pid2);
    sysBlock(pid3);
    sysBlock(pid4);
    fakeSleep();
    printPs();
    fakeSleep();

    printStringWC("Unblockig all test loops\n", BLACK, GREEN);
    sysBlock(pid1);
    sysBlock(pid2);
    sysBlock(pid3);
    sysBlock(pid4);

    printStringWC("Killing loop with pid ", BLACK, GREEN);
    printInt(pid2);
    printStringLn("");
    sysKill(pid2);
    fakeSleep();

    printStringWC("Ending Test\n", BLACK, GREEN);
    sysKill(pid1);
    sysKill(pid3);
    sysKill(pid4);
    fakeSleep();
    printMemoryInfo();
    sysExit();
}



static int createLoop() {
    printStringWC("\nCreating loop in background\n", BLACK, GREEN);
    return sysCreateProcess(&loop, "loop", -1, -1, 1, params);
}

static void fakeSleep() {
    for (int i = 0; i < VERY_BIG_NUMBER * 40; i++)
        ;
}

static void printPs() {
    printStringLn("");
    printStringLn("");
    sysCreateProcess(&ps, "ps", -1, -1, 0, 0);
}

static void printMemoryInfo() {
    sysCreateProcess(&memoryInfo, "memoryInfo", -1, -1, 0, 0);
}