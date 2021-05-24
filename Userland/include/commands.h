#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <shell.h>


//print registers, done in assembly
void inforeg(int argc, char** args);

//recieves memory direction and prints 32 bytes after
void printmem(int argc, char** args);

//displays system time
void time(int argc, char** args);

//shows processor model and brand
void cpuInfo(int argc, char** args);

//shows processor temp
void cpuTemp(int argc, char** args);

//shows arguments
void showArgs(int argc, char** args);

//change to chess game
//void changeToChess(int argc, char** args, t_shellData* shellData);

void memoryInfo(int argc, char** args);

//print all running process info
void ps(int argc, char** args);

void loop(int argc, char** args);

void kill(int argc, char** arg);

void nice(int argc, char** args);

void block(int argc, char** args);

void testSync(int argc, char** args);

void testSyncNoSem(int argc, char** args);

void sem(int argc, char** args);

void cat(int argc, char** args);

void filter(int argc, char** args);

void wc(int argc, char** args);

void phylo(int argc, char** args);

void pipeInfo(int argc, char** args);

#endif