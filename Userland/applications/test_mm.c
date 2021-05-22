#include <test_util.h>
#include <memoryManager.h>
#include <lib.h>
#include <stringLib.h>
#include <syscalls.h>

// #define MAX_MEMORY 1024 * 5000 * 0.8 //Should be around 80% of memory managed by the MM
// #define MAX_BLOCKS 64000

#define MAX_MEMORY 1024 * 1000 * 0.8 //Should be around 80% of memory managed by the MM
#define MAX_BLOCKS 64

typedef struct MM_rq {
    void* address;
    uint32_t size;
}mm_rq;

void test_mm() {
    mm_rq mm_rqs[MAX_BLOCKS];
    //printHex((uint64_t) & (mm_rqs[MAX_BLOCKS - 1]));
    printStringLn("");
    uint8_t rq;
    uint32_t total;

    while (1) {
        rq = 0;
        total = 0;

        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < MAX_MEMORY) {
            mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
            printInt(mm_rqs[rq].size);
            printString(" ");
            mm_rqs[rq].address = malloc(mm_rqs[rq].size); // TODO: Port this call as required
            if (mm_rqs[rq].address == NULL)
                printStringLn("NULL");

            total += mm_rqs[rq].size;
            rq++;
        }
        printStringLn("");

        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address != NULL) {
                printHex((uint64_t)mm_rqs[i].address);
                printString(" ");
                memset(mm_rqs[i].address, i, mm_rqs[i].size); // TODO: Port this call as required
                printInt(*(uint8_t*)(mm_rqs[i].address));
                printStringLn("");

            }

        // Check
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address != NULL) {
                printInt(*(uint8_t*)(mm_rqs[i].address));
                printStringLn("");
                if (memcheck(mm_rqs[i].address, i, mm_rqs[i].size) == 0)
                    printStringWC("ERROR!\n", BLACK, RED); // TODO: Port this call as required
            }

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address != NULL)
                free(mm_rqs[i].address);  // TODO: Port this call as required

        break;
    }
    sysExit();
}