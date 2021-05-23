#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include <buffer.h>
#include <stdint.h>

void keyboardHandler(uint64_t rsp);
//char getchar(int64_t fd);
//char getcharOnce();
void changeBuffer(t_bufferID bufferID);
uint64_t* getSnapshot();

#define MAX_PROCESSES 2

#endif