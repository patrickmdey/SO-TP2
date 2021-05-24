#include <commands.h>
#include <RTCTime.h>
#include <cpuInfo.h>
#include <lib.h>
#include <stringLib.h>
#include <syscalls.h>
#include <utils.h>
#include <registers.h>
#include <chess.h>
#include <memoryManager.h>
#include <test_sync.h>
#include <sem.h>

static char* regNames[] = { "R15: ", "R14: ", "R13: ", "R12: ", "R11: ", "R10: ", "R9: ",
                           "R8: ", "RSI: ", "RDI: ", "RBP: ", "RDX: ", "RCX: ", "RBX: ",
                           "RAX: ", "RIP: ", "RSP: " };


static void memToString(char* buffer, uint8_t* mem, int bytes);

// void changeToChess(int argc, char** args, t_shellData shellData) {
//       sysClear(0, 0, 0, 0);
//       cleanBuffer(&shellData->buffer);
//       sys_changeApp();
// }

//devuelve el tiempo acutal del sistema

void time(int argc, char** args) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            sysExit();
      }

      char dateFormat[3][3];

      uint8_t day = sysRTCTime(DAY);
      uint8_t month = sysRTCTime(MONTH);
      uint8_t year = sysRTCTime(YEAR);
      char aux[9] = { 0 };
      getCurrentTime(aux);
      printString(" >Current time: ");
      printString(aux);
      putchar('\n');
      printString(" >Current date: ");
      intToStr(day, dateFormat[0], 2);
      intToStr(month, dateFormat[1], 2);
      intToStr(year, dateFormat[2], 2);

      for (int i = 0; i < 3; i++) {
            printString(dateFormat[i]);
            if (i != 2) {
                  putchar('/');
            }
      }
      putchar('\n');
      sysExit();
}

//devuelve el modelo y vendedor del cpu
void cpuInfo(int argc, char** args) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            sysExit();
      }
      char vendor[13] = { 0 };
      t_cpuInfo cpuInfo = { vendor, 0 };
      cpuVendor(cpuInfo.cpuVendor);
      cpuInfo.model = cpuModel();
      printString(" > CPU Vendor: ");
      printStringLn(cpuInfo.cpuVendor);
      printString(" > CPU model: ");
      printInt(cpuInfo.model);
      putchar('\n');
      putchar('\n');
      sysExit();
}

//muestra la informacion recoletada sobre los registros obtenidos al haber presionado ctrl + s
void inforeg(int argc, char** args) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            sysExit();
      }
      uint64_t* regData = sysInfoReg();
      for (int i = 0; i < REGISTERS; i++) {
            printString(" > ");
            printString(regNames[i]);
            printHexWL(regData[i], 8);
            putchar('\n');
      }
      putchar('\n');
      sysExit();
}


//Hace un dump de 32 bytes de memria a partir de la direccion pedida
void printmem(int argc, char** args) {
      if (argc != 1) {
            printStringLn("Invalid ammount of arguments.");
            sysExit();
      }

      int error = 0;
      uint64_t memDir = strToHex(args[0], &error);
      if (error) {
            printStringLn("Invalid argument for function printmem (must be a hex value).");
            sysExit();
      }

      int bytes = 32;

      uint8_t memData[bytes];
      sysGetMem(memDir, memData);

      char byteStr[bytes * 2];
      memToString(byteStr, memData, bytes);

      printString(" >Data dump:");
      for (int i = 0; i < 32; i++) {
            if (i % 4 == 0) {
                  putchar('\n');
                  printString("   -[0x");
                  printHex(memDir);
                  printString("]: ");
                  memDir += 4;
            }
            if (i % 2 == 0) {
                  putcharWC(byteStr[i], BLACK, GREEN);
                  putcharWC(byteStr[i + 1], BLACK, GREEN);
                  putchar(' ');
            }
            else {
                  putchar(byteStr[i]);
                  putchar(byteStr[i + 1]);
                  putchar(' ');
            }
      }
      putchar('\n');
      putchar('\n');
}

//Imprime la temperatura actual del cpu
void cpuTemp(int argc, char** args) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            sysExit();
      }
      printString("CPU temp: ");
      printInt(sysTemp());
      printStringLn(" C");
      putchar('\n');
      sysExit();
}

//Muestra los argumentos pasados al comando
void showArgs(int argc, char** args) {
      for (int i = 0; i < argc && i < MAX_ARGS; i++) {
            printString("arg[");
            printInt(i);
            printString("]=");
            printStringLn(args[i]);
      }
      putchar('\n');
      sysExit();
}

void memoryInfo(int argc, char** args) {
      int size = 0;
      char** info = sysGetMeminfo(&size);
      for (int i = 0; i < size; i++) {
            printStringLn(info[i]);
            free(info[i]);
      }
      free(info);
      sysExit();

}

void ps(int argc, char** args) {
      int size = 0;
      char** info = sysPs(&size);
      printStringWC("PID   PRIORITY   STATE     FOREGROUND     RSP             RBP        NAME", BLACK, GREEN);
      printStringLn(" ");
      for (int i = 0; i < size; i++) {
            printStringLn(info[i]);
            free((uint8_t*)info[i]);
      }

      free((uint8_t*)info);
      sysExit();
}

void pipeInfo(int argc, char** args){
      int size = 0;
      char** info = sysPipeInfo(&size);
      if (size == 0) {
            printStringLn("No opened pipes");
            sysExit();
      }
      printStringWC("PROCESS_WAITING   FD", BLACK, GREEN);
      printStringLn(" ");
      for (int i = 0; i < size; i++) {
            printStringLn(info[i]);
            free((void *)info[i]);
      }

      free((void *) info);
      sysExit();
}

void loop(int argc, char** args) {
      int pid = sysGetPid();
      int i;
      while (1) {
            for (i = 0; i < VERY_BIG_NUMBER; i++);
            printInt(pid);
            printString(" ");
      }
      sysExit();
}

void cat(int argc, char** args) {
      char c;
      char toPrint[100];
      int i = 0;
      while (1) {
            c = getchar();
            if (c == '\t') {
                  putchar('\t');
                  putchar('\n');
                  sysExit();
            }
            if (c == '\n') {
                  toPrint[i] = 0;
                  printStringLn(toPrint);
                  i = 0;
            } else {
                  putchar(c);
            }
            toPrint[i++] = c;
      }
}


void filter(int argc, char** args) {
      char c;
      char toPrint[100];
      int i = 0;
      while (1) {
            c = getchar();
            if (c == '\t') {
                  putchar('\t');
                  putchar('\n');
                  sysExit();
            }
            putchar(c);
            if (c == '\n') {
                  toPrint[i] = 0;
                  printStringLn(toPrint);
                  i = 0;
            }
            
            if (IS_VOCAL(c))
                  toPrint[i++] = c;
      }
}

void wc(int argc, char** args) {
      //sysBlock(0);
      char c;
      int i = 0;
      while (1) {
            c = getchar();
            if (c == '\t') {
                  printInt(i);
                  putchar('\t');
                  putchar('\n');
                  sysExit();
            } else {
                  if (c == '\n')
                        i++;
                  putchar(c);
            }
      }
}

void kill(int argc, char** args) {
      if (argc > 1) {
            printStringLn("Too many arguments");
            sysExit();
      }
      int error = 0;
      int pid = strToInt(args[0], &error);
      int killed = sysKill(pid);
      if (killed == 1) {
            printStringLn("Killed process");
      } else if (killed == 0) {
            printString("No process running with pid ");
            printInt(pid);
            printStringLn("");
      } else {
            printString("Can not kill process with pid ");
            printInt(pid);
            printStringLn("");
      }
      sysExit();
}

void sem(int argc, char** args) {
      int size = 0;
      char** info = sysSemInfo(&size);
      if (size == 0) {
            printStringLn("No active semaphores");
            sysExit();
      }
      printStringWC("PROCESS_WAITING   NAME   VALUE   STATE", BLACK, GREEN);
      printStringLn(" ");
      for (int i = 0; i < size; i++) {
            printStringLn(info[i]);
            free((uint8_t*)info[i]);
      }

      free((uint8_t*)info);
      sysExit();
}

void nice(int argc, char** args) {
      int error = 0;
      int pid = strToInt(args[0], &error);
      if (error) {
            sysExit();
      }
      int priority = strToInt(args[1], &error);
      if (error) {
            sysExit();
      }
      error = sysNice(pid, priority);
      if (error == 0) {
            printStringLn("Not found");
      }

      sysExit();
}

void block(int argc, char** args) {
      int error = 0;
      int pid = strToInt(args[0], &error);
      if (error) {
            sysExit();
      }
      if(pid == 0 || pid == 1){
            printString("Can not change state of process with pid ");
            printInt(pid);
            printStringLn("");
            sysExit();
      }

      int blocked = sysBlock(pid);
      if (blocked) {
            printString("Changed process ");
            printInt(pid);
            printStringLn(" state");
      } else {
            printString("No process running with pid ");
            printInt(pid);
            printStringLn("");
      }

      sysExit();
}

static void memToString(char* buffer, uint8_t* mem, int bytes) {
      for (int i = 0; i < bytes * 2; i++) {
            if (mem[i] <= 0xF) {
                  buffer[i] = '0';
                  uintToBase(mem[i], buffer + i + 1, 16);
            }
            else {
                  uintToBase(mem[i], buffer + i, 16);
            }
      }
      sysExit();
}