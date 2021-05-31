// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include <tests.h>
#include <phylo.h>

#define MAX_PIPE_PROCESS 2

static void initShell(t_shellData* shellData);
static int processCommand(t_shellData* shellData);
static void processChar(char c, t_shellData* shellData);


static void help(t_shellData* shellData);
static void checkZeroException(t_shellData* shellData);
static void checkInvalidOpcodeException(t_shellData* shellData);

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
          {NULL, &test_prio, "testPrio", "tests changing processes using priority", 0},
          {NULL, &test_processes, "testProcesses", "tests changing processes", 0},
          {NULL, &test_mm, "testMM", "tests memory manager", 0},
          {NULL, &testContextSwitching, "testContextSwitching", "tests kernel context switching with priorities", 0},
          {NULL, &sem, "sem", "prints a list with all opened semaphores with their most relevant information", 0},
          {NULL, &cat, "cat", "prints to stdout the content of the fd. Use tab to go back", 0},
          {NULL, &filter, "filter", "prints the vocals to stdout the content of the fd. Use tab to go back", 0},
          {NULL, &wc, "wc", "counts the amount of lines in a given input. Use tab get result and go back", 0},
          {NULL, &phylo, "phylo", "simulates the phylosopher's table problem. Use tab to go back", 0},
          {NULL, &pipeInfo, "pipe", "prints a list with all opened pipes with their most relevant information", 0},
          {NULL, &shmTestProcess, "testShm", "simulates use of shared memory between processes", 0}
      };

      for (int i = 0; i < COMMANDS; i++) {
            shellData->commands[i].builtIn = commandsData[i].builtIn;
            shellData->commands[i].command = commandsData[i].command;
            shellData->commands[i].name = commandsData[i].name;
            shellData->commands[i].description = commandsData[i].description;
            shellData->commands[i].isBuiltIn = commandsData[i].isBuiltIn;
      }

      cleanBuffer(&shellData->buffer);
      printPrompt();
}

// procesa el caracter recibido actua segun el mismo
static void processChar(char c, t_shellData* shellData) {
      int printsPrompt;
      if (c != 0) {
            switch (c) {
            case CLEAR_SCREEN:
                  sysClear(0, 0, 0, 0);
                  cleanBuffer(&shellData->buffer);
                  printPrompt();
                  break;
            case '\n':
                  putchar('\n');
                  printsPrompt = processCommand(shellData);
                  cleanBuffer(&shellData->buffer);
                  if (printsPrompt)
                        printPrompt();
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
static int processCommand(t_shellData* shellData) {
      uint8_t argc[MAX_PIPE_PROCESS] = { 0 };
      char** argv[MAX_PIPE_PROCESS];

      int i, j, k;

      char command[MAX_PIPE_PROCESS][BUFFER_SIZE] = { {0} };

      char process[MAX_PIPE_PROCESS][BUFFER_SIZE] = { {0} };
      int idx = 0;

      strtok(0, 0, ' ');
      strtok(shellData->buffer.buffer, process[idx], '|');
      while (idx < MAX_PIPE_PROCESS && strtok(NULL, process[idx], '|')) {
            idx++;
      }

      for (i = 0; i < idx; i++) {
            argv[i] = (char**)malloc(MAX_ARGS * sizeof(char*));
            if (argv[i] == NULL) {
                  printStringWC("Unable to create process\n", BLACK, RED);
                  for (j = 0; j < i; j++) {
                        for (k = 0; k < argc[j]; k++) {
                              free(argv[j][k]);
                        }
                        free(argv[j]);
                  }
                  return 1;
            }
            argc[i] = 0;
            strtok(0, 0, ' ');
            strtok(process[i], command[i], ' ');    // parse buffer
            strtok(NULL, command[i], ' ');          // parse buffer


            argv[i][0] = (char *) malloc(BUFFER_SIZE * sizeof(char));
            if (argv[i][0] == NULL) {
                  printStringWC("Unable to create process\n", BLACK, RED);
                  for (j = 0; j < i; j++) {
                        for (k = 0; k < argc[j]; k++) {
                              free(argv[j][k]);
                        }
                        free(argv[j]);
                  }
                  free(argv[i]);
                  return 1;
            }
            while (argc[i] < MAX_ARGS && strtok(NULL, argv[i][argc[i]], ' ')) {
                  argc[i]++;
                  argv[i][argc[i]] = (char*)malloc(BUFFER_SIZE * sizeof(char));
            };
            if (argc[i] != MAX_ARGS)
                  free(argv[i][argc[i]]);

      }

      strtok(0, 0, ' ');

      k = 0;
      int64_t fd[MAX_PIPE_PROCESS][2] = {
            {-1, -1},
            {-1, -1}
      };

      int64_t pids[MAX_PIPE_PROCESS] = { -1 , -1 };
      int openedFd = -1;
      if (idx == MAX_PIPE_PROCESS) {
            openedFd = sysGetFd();
            fd[0][1] = openedFd;
            fd[1][0] = openedFd;
      }
      int notFound = 1;
      //int invalidCommand[MAX_PIPE_PROCESS] = { 1, 1 };
      int toExecute[MAX_PIPE_PROCESS] = { -1, -1 };

      for (i = 0; i < COMMANDS && notFound; i++) {
            for (j = 0; j < idx; j++) {
                  if (stringcmp(shellData->commands[i].name, command[j]) == 0) {
                        //invalidCommand[j] = 0;
                        /*if (shellData->commands[i].isBuiltIn) {
                              free(argv[j]);
                              shellData->commands[i].builtIn(shellData);
                        }
                        else {
                              pids[j] = sysCreateProcess(shellData->commands[i].command,
                                    shellData->commands[i].name, fd[j][0], fd[j][1], argc[j], argv[j]);
                        }*/
                        toExecute[j] = i;
                        k++;
                        if (k == idx) {
                              notFound = 0;
                              break;
                        }
                  }
            }

      }

      for (i = 0; i < idx; i++) {
            if (toExecute[i] == -1) {
                  printString("Invalid command: ");
                  printStringLn(command[i]);
                  for (j = 0; j < idx; j++) {
                        for (k = 0; k < argc[j]; k++) {
                              free(argv[j][k]);
                        }
                        free(argv[j]);
                  }
                  return 1;
            }
      }

      for (i = 0; i < idx; i++) {
            int execute = toExecute[i];
            if (shellData->commands[execute].isBuiltIn) {
                  free(argv[i]);
                  shellData->commands[execute].builtIn(shellData);
            }
            else {
                  pids[i] = sysCreateProcess(shellData->commands[execute].command,
                        shellData->commands[execute].name, fd[i][0], fd[i][1], argc[i], argv[i]);
            }
      }

      for (i = 0; i < idx; i++) {
            if (pids[i] != -1 && (argc[i] == 0 && argv[i][argc[i] - 1][0] != '&')) {
                  sysWaitpid(pids[i]); // cedo foreground si cree un proceso y este no esta en el background
            }
      }
      if (openedFd != -1) {
            sysCloseFd(openedFd);
      }

      return 1;
}

//muestra la lista de comandos con sus descripciones
static void help(t_shellData* shellData) {
      printStringLn("These shell commands are defined internally.  Type 'help' to see this list.");
      for (int i = 0; i < COMMANDS; i++) {
            printString(" >");
            printStringWC(shellData->commands[i].name, BLACK, GREEN);
            printString(": ");
            printStringLn(shellData->commands[i].description);
      }
      putchar('\n');
}

//causa una excepcion de dividir por cero
static void checkZeroException(t_shellData* shellData) {
      check0Exception();
}

//causa una excepcion de tipo invalid opcode
static void checkInvalidOpcodeException(t_shellData* shellData) {
      __asm__("ud2");  // https://hjlebbink.github.io/x86doc/html/UD2.html
}
