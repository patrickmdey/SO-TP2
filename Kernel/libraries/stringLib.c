#include <stringLib.h>
#include <videoDriver.h>
#include <utils.h>
#include <pipe.h>
#include <staticQueue.h>
#include <taskManager.h>

void sysWrite(char* string, uint8_t lenght, t_colour bgColour, t_colour fontColour) {
      if (lenght <= 0 || string == 0 || bgColour < 0 || fontColour < 0) {
            return;
      }

      uint64_t fd = getCurrentOut();
      if (fd != STDOUT) {
            t_fdNode* node = findFd(fd);
            pipeWriteStr(node, string);
      } else {
            for (int i = 0; string[i] != 0 && i < lenght; i++) {
                  if (string[i] == '\n') {
                        changeLineOnScreen();
                  } else if (string[i] == '\b') {
                        removeCharFromScreen();
                  } else {
                        printCharOnScreen(string[i], bgColour, fontColour, 1);
                  }
            }
      }
}

void sysStaticWrite(char* string, uint8_t lenght, t_colour bgColour, t_colour fontColour) {
      if (lenght <= 0 || string == 0 || bgColour < 0 || fontColour < 0) {
            return;
      }

      for (int i = 0; string[i] != 0 && i < lenght; i++) {
            printCharOnScreen(string[i], bgColour, fontColour, 0);
      }
}

void printString(char* str) {
      sysWrite(str, strlen(str), BLACK, WHITE);
}

void printStringWC(char* str, t_colour bgColour, t_colour fontColour) {
      sysWrite(str, strlen(str), bgColour, fontColour);
}

void printStringLn(char* str) {
      printString(str);
      putchar('\n');
}

void putchar(char c) {
      sysWrite(&c, 1, BLACK, WHITE);
}

void printHex(uint64_t num) {
      char buffer[10];
      uintToBase(num, buffer, 16);
      printString(buffer);
}

void printHexWLC(uint64_t num, int lenght, t_colour bgColour, t_colour fontColour) {
      char buffer[10];
      uintToBaseWL(num, buffer, 16, 8);
      printStringWC(buffer, bgColour, fontColour);
}

void printHexWC(uint64_t num, t_colour bgColour, t_colour fontColour) {
      char buffer[10];
      uintToBase(num, buffer, 16);
      printStringWC(buffer, bgColour, fontColour);
}

void printInt(int64_t num) {
      char buffer[10];
      uintToBase(num, buffer, 10);
      printString(buffer);
}

void clear() {
      clearScreen();
}

void staticputchar(char c) {
      sysStaticWrite(&c, 1, BLACK, WHITE);
}
