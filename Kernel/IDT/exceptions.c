#include <lib.h>
#include <stringLib.h>
#include <taskManager.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6
#define REGISTERS 16

static void zeroDivision();
static void invalidOpcode();
static void printRegisters(uint64_t* registers);

static char* regNames[] = { "R15: ", "R14: ", "R13: ", "R12: ", "R11: ", "R10: ", "R9: ",
                           "R8: ", "RSI: ", "RDI: ", "RBP: ", "RDX: ", "RCX: ", "RBX: ",
                           "RAX: ", "RIP: ", "RSP: " };

//Maneja la excepcion recibida y reinicia el proceso actual
void exceptionDispatcher(int exception, void* stackframe) {
      putchar('\n');
      switch (exception) {
      case ZERO_EXCEPTION_ID:
            zeroDivision();
            break;

      case INVALID_OPCODE_ID:
            invalidOpcode();
            break;
      }
      printRegisters(stackframe);
      putchar('\n');
      resetCurrentProcess();
}

static void zeroDivision() {
      printStringWC("Exception 0: division by zero is undefined\n", BLACK, RED);
}

static void invalidOpcode() {
      printStringWC("Exception 6: invalid opcode\n", BLACK, RED);
}

static void printRegisters(uint64_t* registers) {
      for (int i = 0; i < REGISTERS; i++) {
            printStringWC(regNames[i], BLACK, RED);
            printHexWLC(registers[i], 8, BLACK, RED);
            putchar('\n');
      }
      printStringWC(regNames[REGISTERS], BLACK, RED);
      printHexWLC(registers[15 + 3], 8, BLACK, RED);  //print rsp value from interrupt stackframe
}