#include <commands.h>
#include <RTCTime.h>
#include <cpuInfo.h>
#include <lib.h>
#include <stringLib.h>
#include <syscalls.h>
#include <utils.h>
#include <chess.h>
#include <memoryManager.h>
#include <test_sync.h>

#define VERY_BIG_NUMBER 9999999

static void memToString(char* buffer, uint8_t* mem, int bytes);

void changeToChess(int argc, char** args, t_shellData* shellData) {
      sysClear(0, 0, 0, 0);
      cleanBuffer(&shellData->buffer);
      sys_changeApp();
}

//devuelve el tiempo acutal del sistema
void time(int argc, char** args, t_shellData* shellData) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
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
}

//devuelve el modelo y vendedor del cpu
void cpuInfo(int argc, char** args, t_shellData* shellData) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
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
}

//Hace un dump de 32 bytes de memria a partir de la direccion pedida
void printmem(int argc, char** args, t_shellData* shellData) {
      if (argc != 1) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
      }

      int error = 0;
      uint64_t memDir = strToHex(args[0], &error);
      if (error) {
            printStringLn("Invalid argument for function printmem (must be a hex value).");
            putchar('\n');
            return;
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
                  putcharWC(byteStr[i], BLACK, BLUE);
                  putcharWC(byteStr[i + 1], BLACK, BLUE);
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
void cpuTemp(int argc, char** args, t_shellData* shellData) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
      }
      printString("CPU temp: ");
      printInt(sysTemp());
      printStringLn(" C");
      putchar('\n');
}

//causa una excepcion de dividir por cero
void checkZeroException(int argc, char** args, t_shellData* shellData) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
      }
      check0Exception();
}

//causa una excepcion de tipo invalid opcode
void checkInvalidOpcodeException(int argc, char** args, t_shellData* shellData) {
      if (argc != 0) {
            printStringLn("Invalid ammount of arguments.");
            putchar('\n');
            return;
      }
      __asm__("ud2");  // https://hjlebbink.github.io/x86doc/html/UD2.html
}

//Muestra los argumentos pasados al comando
void showArgs(int argc, char** args, t_shellData* shellData) {
      for (int i = 0; i < argc && i < MAX_ARGS; i++) {
            printString("arg[");
            printInt(i);
            printString("]=");
            printStringLn(args[i]);
      }
      putchar('\n');
}

void memoryInfo(int argc, char** args, t_shellData* shellData) {
      int size = 0;
      char** info = sysGetMeminfo(&size);
      for (int i = 0; i < size;i++) {
            printStringLn(info[i]);
            free(info[i]);
      }
      free(info);

}

void ps(int argc, char** args, t_shellData* shellData) {
      int size = 0;
      char** info = sysPs(&size);
      printStringWC("PID   PRIORITY   STATE     FOREGROUND     RSP             RBP        NAME", BLACK, GREEN);
      printStringLn(" ");
      for (int i = 0; i < size; i++) {
            printStringLn(info[i]);
            free((uint8_t*)info[i]);
      }

      free((uint8_t*)info);
}

static void loopProcess() {
      int pid = sysGetPid();
      int i;
      while (1) {
            for (i = 0; i < VERY_BIG_NUMBER; i++);
            printInt(pid);
            printString(" ");
      }
}

void loop(int argc, char** args, t_shellData* shellData) {
      sysCreateProcess(&loopProcess, "loop", argc, args);
}

static void catProcess() {
      sysBlock(0);
      char c;
      char toPrint[100];
      int i = 0;
      while (1) {
            c = getchar();
            if (c == '\t') {
                  sysBlock(0);
                  ps(0, 0, 0);
                  sysExit();
            }
            putchar(c);
            if (c == '\n') {
                  toPrint[i] = 0;
                  printStringLn(toPrint);
                  i = 0;
            }
            toPrint[i++] = c;
      }
}

void cat(int argc, char** args, t_shellData* shellData) {
      sysCreateProcess(&catProcess, "cat", argc, args);
}

static void filterProcess() {
      sysBlock(0);
      char c;
      char toPrint[100];
      int i = 0;
      while (1) {
            c = getchar();
            if (c == '\t') {
                  ps(0, 0, 0);
                  sysBlock(0);
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

void filter(int argc, char** args, t_shellData* shellData) {
      sysCreateProcess(&filterProcess, "filter", argc, args);
}

void wcProcess() {
      sysBlock(0);
      char c;
      int i = 0;
      while (1) {
            c = getchar();
            if (c == '\t') {
                  sysBlock(0);
                  sysExit();
            }
            else if (c == '#') {
                  printInt(i);
                  i = 0;
                  putchar('\n');
            }
            else {
                  if (c == '\n')
                        i++;
                  putchar(c);
            }
      }
}

void wc(int argc, char** args, t_shellData* shellData) {
      sysCreateProcess(&wcProcess, "wc", argc, args);
}

void kill(int argc, char** args, t_shellData* shellData) {
      if (argc > 2) {
            printStringLn("To many arguments");
            return;
      }
      int error = 0;
      int pid = strToInt(args[0], &error);
      int killed = sysKill(pid);
      if (killed == 1) {
            printStringLn("Killed process");
      }
      else if (killed == 0) {
            printString("No process running with pid ");
            printInt(pid);
            printStringLn("");
      }
      else {
            printStringLn("Error occured: no processes running");
      }
}

void sem(int argc, char** args, t_shellData* shellData) {
      int size = 0;
      char** info = sysSemInfo(&size);
      if (size == 0) {
            printStringLn("No active semaphores");
            return;
      }
      printStringWC("PROCESS_WAITING   NAME   VALUE   STATE", BLACK, GREEN);
      printStringLn(" ");
      for (int i = 0; i < size; i++) {
            printStringLn(info[i]);
            free((uint8_t*)info[i]);
      }

      free((uint8_t*)info);
}

void testSync(int argc, char** args, t_shellData* shellData) {
      sysCreateProcess(&test_sync, "test-sync", argc, args);
}

void testSyncNoSem(int argc, char** args, t_shellData* shellData) {
      sysCreateProcess(&test_no_sync, "test-sync-n", argc, args);
}

void nice(int argc, char** args, t_shellData* shellData) {
      int error = 0;
      int pid = strToInt(args[0], &error);
      if (error) {
            return;
      }
      int priority = strToInt(args[1], &error);
      if (error) {
            return;
      }
      error = sysNice(pid, priority);
      if (error == 0) {
            printStringLn("Not found");
      }
}

void block(int argc, char** args, t_shellData* shellData) {
      int error = 0;
      int pid = strToInt(args[0], &error);
      if (error) {
            return;
      }
      int blocked = sysBlock(pid);
      if (blocked) {
            printString("Changed process ");
            printInt(pid);
            printStringLn(" state");
      }
      else
            printStringLn("Couldn´t change process state");
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
}