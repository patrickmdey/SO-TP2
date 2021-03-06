#ifndef SHELL_H
#define SHELL_H

#include <buffer.h>

#define COMMANDS 27
#define MAX_ARGS 4

typedef struct t_command t_command;
typedef struct t_shellData t_shellData;

typedef struct t_command {
      void (* builtIn)(t_shellData * shellData);
      void * command;
      char* name;
      char* description;
      uint8_t isBuiltIn;
} t_command;

typedef struct t_shellData {
      t_command commands[COMMANDS];
      t_buffer buffer;
} t_shellData;

void runShell();

#endif