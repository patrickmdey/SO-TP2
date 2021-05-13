#include <keyboardDriver.h>
#include <stdint.h>
#include <timerTick.h>

#include <lib.h>

#define TIMER_TICK 0
#define KEYBOARD 1

static void int_20();
static void int_21(uint64_t rsp);

void irqDispatcher(uint64_t irq, uint64_t rsp) {
      switch (irq) {
      case TIMER_TICK:
            int_20();
            break;

      case KEYBOARD:
            int_21(rsp);
            break;
      }
}

static void int_20() {
      timerHandler();
      // if (ticksElapsed() % 32 == 0) {
            sys_changeProcess();
      //}

}

static void int_21(uint64_t rsp) {
      keyboardHandler(rsp);
}
