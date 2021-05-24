#include <keyboardDriver.h>
#include <stdint.h>

#include <lib.h>

#define TIMER_TICK 0
#define KEYBOARD 1

static void int_21(uint64_t rsp);

void irqDispatcher(uint64_t irq, uint64_t rsp) {
      switch (irq) {

      case KEYBOARD:
            int_21(rsp);
            break;
      }
}

static void int_21(uint64_t rsp) {
      keyboardHandler(rsp);
}
