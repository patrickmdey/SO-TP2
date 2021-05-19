#ifndef TEST_SYNC_H
#define TEST_SYNC_H

#include <stdint.h>

void test_sync(int argc, char ** argv);
void test_no_sync(int argc, char ** argv);
void inc(int argc, char ** argv);
void slowInc(int64_t* p, int64_t inc);

#endif