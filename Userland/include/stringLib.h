#ifndef STRING_LIB_H
#define STRING_LIB_H

#include <stdint.h>
#include <colours.h>

void printString(char* str);
void printStringLn(char* str);
void printStringWC(char* str, t_colour bgColour, t_colour fontColour);
void printInt(int64_t num);
void printDouble(double num);
void printHex(uint64_t num);
void printHexWL(uint64_t num, int length);
void putchar(char c);
void putcharWC(char c, t_colour bgColour, t_colour fontColour);
void staticputchar(char c);
char readchar(int64_t fd);
char getchar();
void deletechar();
int strlen(char* str);
void strcpy(char* dst, char* src);

#endif