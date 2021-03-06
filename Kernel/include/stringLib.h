#ifndef STRING_LIB_H
#define STRING_LIB_H

#include <stdint.h>
#include <colours.h>

void sysWrite(int64_t fd, char* string, uint8_t lenght, t_colour bgColour, t_colour fontColour);
void sysStaticWrite(char* string, uint8_t lenght, t_colour bgColour, t_colour fontColour);
void printString(char* str);
void printStringLn(char* str);
void printStringWC(char* str, t_colour bgColour, t_colour fontColour);
void printInt(int64_t num);
void printHex(uint64_t num);
void printHexWLC(uint64_t num, int lenght, t_colour bgColour, t_colour fontColour);
void printHexWC(uint64_t num, t_colour bgColour, t_colour fontColour);
void putchar(char c);
void putcharWC(char c, t_colour bgColour, t_colour fontColour);
void staticputchar(char c);
int strlen(char* str);

#endif