// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stringLib.h>
#include <syscalls.h>
#include <utils.h>

void printString(char* str) {
      syscall(WRITE, -1, (uint64_t) str, strlen(str), BLACK, WHITE, 0);
}

void printStringWC(char* str, t_colour bgColour, t_colour fontColour) {
      syscall(WRITE, -1, (uint64_t) str, strlen(str), bgColour, fontColour, 0);
}

void printStringLn(char* str) {
      printString(str);
      putchar('\n');
}

char readchar(int64_t fd) {
      return syscall(GETCHAR, fd, 0, 0, 0, 0, 0);
}

char getchar() {
      return readchar(-1);
}

void putchar(char c) {
      syscall(WRITE, -1, (uint64_t) &c, 1, BLACK, WHITE, 0);
}

void putcharWC(char c, t_colour bgColour, t_colour fontColour) {
      syscall(WRITE, -1, (uint64_t) &c, 1, bgColour, fontColour, 0);
}

void printHex(uint64_t num) {
      char buffer[10];
      uintToBase(num, buffer, 16);
      printString(buffer);
}

void printHexWL(uint64_t num, int length) {
      char buffer[10];
      uintToBaseWL(num, buffer, 16, 8);
      printString(buffer);
}

void printInt(int64_t num) {
      char buffer[100];
      uintToBase(num, buffer, 10);
      printString(buffer);
}

void clear() {
      sysClear(0,0,0,0);
}

void deletechar() {
      putchar('\b');
}

void strcpy(char* dst, char* src) {
      int i;
      for (i = 0; src[i] != 0; i++) {
            dst[i] = src[i];
      }
      dst[i] = 0;
}
