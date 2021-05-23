#include <keyboardDriver.h>

//dataTypes
#include <buffer.h>
#include <keys.h>

//functions
#include <interrupts.h>
#include <keyboardInfo.h>
#include <lib.h>
#include <staticQueue.h>
#include <utils.h>
#include <videoDriver.h>

#include <taskManager.h>

#define MAX_PROCESSES 2
#define REGISTERS 16

static uint8_t action(uint8_t scanCode);
static void updateSnapshot(uint64_t* rsp);
static void processKey(uint8_t scanCode, uint8_t *currentAction, 
      uint8_t *specialChars, uint8_t *capsLock, uint8_t *leftCtrl, uint64_t *rsp);

static char pressCodes[KEYS][2] =
    {{0, 0}, {0, 0}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'}, {'\t', '\t'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, {0, 0}, {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'}, {'\'', '\"'}, {'`', '~'}, {0, 0}, {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, {0, 0}, {0, 0}, {0, 0}, {' ', ' '}, {0, 0}};

static uint8_t scanCode, currentAction, specialChars = 0, capsLock = 0, leftCtrl = 0;
static t_specialKeyCode clearS = CLEAR_SCREEN;
static uint64_t registers[REGISTERS+1] = {0};

void keyboardHandler(uint64_t rsp) {
      if (hasKey()) {
            scanCode = getKey();
            currentAction = action(scanCode);
            processKey(scanCode, &currentAction, &specialChars, &capsLock, &leftCtrl, &rsp);
      }
}

/*char getchar(int64_t fd) {
      char c = 0;
      while (c == 0) {
            if (ticksElapsed() % 12 == 0) {
                  blinkCursor();
            }
            _hlt();
            c = readChar(fd);
      
      }
      stopBlink();
      return readChar(fd);
}*/

/*char getcharOnce(){
      char c = 0;
      if (ticksElapsed() % 12 == 0) {
             blinkCursor();
      }
      _hlt();
      c = removeKeyFromBuffer();
      stopBlink();
      return c;
}*/

uint64_t* getSnapshot() {
      return registers;
}

static void updateSnapshot(uint64_t* rsp) {
      int i;
      for (i = 0; i < REGISTERS; i++) {
            registers[i] = rsp[i];
      }
      registers[i] = rsp[15 + 3];  //load rsp manualy
}

static uint8_t action(uint8_t scanCode) {
      if (scanCode >= 0x01 && scanCode <= 0x3A)
            return PRESSED;
      else if (scanCode >= 0x81 && scanCode <= 0xBA)
            return RELEASED;

      return ERROR;
}

static void processKey(uint8_t scanCode, uint8_t *currentAction, uint8_t *specialChars, uint8_t *capsLock, uint8_t *leftCtrl, uint64_t *rsp) {
    if (*currentAction == PRESSED) {
        switch (scanCode) {
        case L_SHFT_SC:
        case R_SHFT_SC:
            *specialChars = 1;
            break;

        case CAPS_LCK_SC:
            *capsLock = *capsLock == 1 ? 0 : 1;
            break;

        case L_CONTROL_SC:
            *leftCtrl = 1;
            break;

        default:
            if (pressCodes[scanCode][0] != 0) {
                if (*leftCtrl) {
                    if (pressCodes[scanCode][0] == 'l') {
                        //queueInsert(currentBuffer, &clearS);
                        writeKeyOnBuffer(clearS);
                    }
                    else if (pressCodes[scanCode][0] == 's') {
                        updateSnapshot(rsp);
                    }
                }
                else {
                    if (!IS_LETTER(pressCodes[scanCode][0])) {
                        //queueInsert(currentBuffer, &pressCodes[scanCode][specialChars]);
                        writeKeyOnBuffer(pressCodes[scanCode][*specialChars]);
                    }
                    else {
                        //queueInsert(currentBuffer, &pressCodes[scanCode][ABS(capsLock - (specialChars))]);
                        writeKeyOnBuffer(pressCodes[scanCode][ABS(*capsLock - (*specialChars))]);
                    }
                }
            }
        }
    }
    else if (*currentAction == RELEASED) {
        switch (scanCode) {
        case L_SHFT_SC | 0x80:  //for realease code
        case R_SHFT_SC | 0x80:
            *specialChars = 0;
            break;

        case L_CONTROL_SC | 0x80:
            *leftCtrl = 0;
            break;
        }
    }
}
