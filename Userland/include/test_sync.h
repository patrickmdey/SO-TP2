#ifndef TEST_SYNC_H
#define TEST_SYNC_H

#include <stdint.h>

void test_sync();
void test_no_sync();
void inc(char * semC, char * valueC, char * NC);
void slowInc(int64_t* p, int64_t inc);

#endif