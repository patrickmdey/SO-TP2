#include <shell.h>
#include <commands.h>
#include <keys.h>
#include <lib.h>
#include <registers.h>
#include <stdint.h>
#include <stringLib.h>
#include <syscalls.h>
#include <utils.h>

#include <stdint.h>
#include <stddef.h> 

#include <memoryManager.h>
#include <test_sync.h>

#define MAX_PIPE_PROCESS 2

static void initShell(t_shellData* shellData);
static void shellText(t_shellData* shellData);
static void processCommand(t_shellData* shellData);
static void processChar(char c, t_shellData* shellData);

void runShell() {
      t_shellData shellData;
      initShell(&shellData);
      char c;

      while (1) {
            c = getchar();
            processChar(c, &shellData);
      }
      sysExit();
}


//inicia la shell y todas sus estructuras 
static void initShell(t_shellData* shellData) {
      t_command commandsData[] = {
          {&help, NULL, "help", "shows the list of commands and their use", 1},
          {&changeUsername, NULL, "changeUsername", "changes the shell prompt username", 1},
          {&checkZeroException, NULL, "checkZeroException", "triggers a zero division exception", 1},
          {&checkInvalidOpcodeException, NULL, "checkInvalidOpcodeException", "triggers an invalid opcode exception", 1},
          {NULL, &inforeg, "inforeg", "prints the value of all the registers on screen, press ctrl + s to update values", 0},
          {NULL, &printmem, "printmem", "recieves an hexadecimal direction and makes a memory dump of 32 bytes on screen", 0},
          {NULL, &time, "time", "prints the current system time on screen", 0},
          {NULL, &cpuInfo, "cpuInfo", "prints the processor brand and model on screen", 0},
          {NULL, &cpuTemp, "cpuTemp", "prints the current processor temperature on screen", 0},
          {NULL, &showArgs, "showArgs", "prints the arguments passed to this command", 0},
          /*{&changeToChess, "chess", "starts or resumes a chess game"},*/
          {NULL, &memoryInfo, "memoryInfo", "prints information of memory manager status", 0},
          {NULL, &ps, "ps", "prints a list with all running processes with their most relevant information", 0},
          {NULL, &loop, "loop", "creates loop process", 0},
          {NULL, &kill, "kill", "kills the process with the given pid", 0},
          {NULL, &block, "block", "changes process state between blocked and ready with given pid", 0},
          {NULL, &nice, "nice", "changes the process with the given pid priority to the new priority", 0},
          {NULL, &test_sync, "testSync", "tests syncronization using semaphores", 0},
          {NULL, &test_no_sync, "testSyncNoSem", "tests syncronization without using semaphores", 0},
          {NULL, &sem, "sem", "prints a list with all opened semaphores with their most relevant information", 0},
          {NULL, &cat, "cat", "prints to stdout the content of the fd", 0},
          {NULL, &filter, "filter", "prints the vocals to stdout the content of the fd", 0},
          {NULL, &wc, "wc", "counts the amount of lines in a given input", 0}

      };

      for (int i = 0; i < COMMANDS; i++) {
            shellData->commands[i].builtIn = commandsData[i].builtIn;
            shellData->commands[i].command = commandsData[i].command;
            shellData->commands[i].name = commandsData[i].name;
            shellData->commands[i].description = commandsData[i].description;
            shellData->commands[i].isBuiltIn = commandsData[i].isBuiltIn;
      }

      cleanBuffer(&shellData->buffer);
      strcpy(shellData->username, "USER");
      shellText(shellData);
}

// procesa el caracter recibido actua segun el mismo
static void processChar(char c, t_shellData* shellData) {
      if (c != 0) {
            switch (c) {
            case CLEAR_SCREEN:
                  sysClear(0,0,0,0);
                  cleanBuffer(&shellData->buffer);
                  shellText(shellData);
                  break;
            case '\n':
                  putchar('\n');
                  processCommand(shellData);
                  cleanBuffer(&shellData->buffer);
                  shellText(shellData);
                  break;
            case '\b':
                  if (shellData->buffer.index > 0) {
                        shellData->buffer.buffer[--shellData->buffer.index] = 0;
                        deletechar();
                  }
                  break;
            default:
                  if (shellData->buffer.index < BUFFER_SIZE) {
                        shellData->buffer.buffer[shellData->buffer.index++] = c;
                        putchar(c);
                  }
            }
      }
}

//procesa el comando, tokenizando lo ingresado.
static void processCommand(t_shellData * shellData) {
      int argc[MAX_PIPE_PROCESS] = {0};
      char ** argv[MAX_PIPE_PROCESS];

      int i, j, k;
      
      char command[MAX_PIPE_PROCESS][BUFFER_SIZE] = {{0}};

      char process[MAX_PIPE_PROCESS][BUFFER_SIZE] = {{0}};
      int idx = 0;

      strtok(0, 0, ' ');
      strtok(shellData->buffer.buffer, process[idx], '|');
      while (idx < MAX_PIPE_PROCESS && strtok(NULL, process[idx], '|')) {
            idx++;
      }

      for (i = 0; i < idx; i++) {
            argv[i] = (char **) malloc(MAX_ARGS * sizeof(char *));
            argc[i] = 0;
            strtok(0, 0, ' ');
            strtok(process[i], command[i], ' ');    // parse buffer
            strtok(NULL, command[i], ' ');          // parse buffer


            argv[i][0] = (char *) malloc(BUFFER_SIZE * sizeof(char));
            while (argc[i] < MAX_ARGS && strtok(NULL, argv[i][argc[i]], ' ')) {
                  argc[i]++;
                  argv[i][argc[i]] = (char *) malloc(BUFFER_SIZE * sizeof(char));
            };
            if (argc[i] != MAX_ARGS)
                  free(argv[i][argc[i]]);

      }

      strtok(0, 0, ' ');

      k = 0; 
      // aa | bb
      // aa[STDIN, fd]
      // bb[fd, STDOUT]
      int64_t fd[MAX_PIPE_PROCESS][2] = {
            {-1, -1}, 
            {-1, -1}
      };
      if (idx == MAX_PIPE_PROCESS) {
            int aux = sysGetFd();
            fd[0][1] = aux;
            fd[1][0] = aux;
      }
      for (i = 0; i < COMMANDS; i++) {
            for (j = 0; j < idx; j++) {
                  if (stringcmp(shellData->commands[i].name, command[j]) == 0) {
                        if (shellData->commands[i].isBuiltIn) {
                              shellData->commands[i].builtIn(argc[j], argv[j], shellData);
                        } else {
                              sysCreateProcess(shellData->commands[i].command, 
                                    shellData->commands[i].name, fd[j][0], fd[j][1], argc[j], argv[j]);
                              sysYield();
                        }
                        k++;
                        if (k == idx) {
                              return;
                        }
                  }
            }
      }
      printStringLn("Invalid command");
}


//muestra en pantalla el texto de la shell
static void shellText(t_shellData* shellData) {
      printStringWC(shellData->username, BLACK, WHITE);
      printStringWC(" $ > ", BLACK, WHITE);
}

//cambia el nombre del usuario mostrado en la shell
void changeUsername(int argc, char** argv, t_shellData* shellData) {
      if (argc != 1) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
      }
      cleanString(shellData->username);
      strcpy(shellData->username, argv[0]);
}

//muestra la lista de comandos con sus descripciones
void help(int argc, char** args, t_shellData* shellData) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
      }

      printStringLn("These shell commands are defined internally.  Type 'help' to see this list.");
      for (int i = 0; i < COMMANDS; i++) {
            printString(" >");
            printStringWC(shellData->commands[i].name, BLACK, BLUE);
            printString(": ");
            printStringLn(shellData->commands[i].description);
      }
      putchar('\n');
}
