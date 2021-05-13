#include <staticStack.h>
#include <utils.h>

void stackPop(t_stack* stack, void* elem) {
      if (!(stackIsEmpty(stack))) {
            memcpy(elem, (void*)((uint64_t)stack->stack + ((stack->size - 1) * stack->dataSize)), stack->dataSize);
            stack->size--;
      }
}

void stackPush(t_stack* stack, void* elem) {
      if (stack->size < stack->dim) {
            memcpy((void*)((uint64_t)stack->stack + (stack->size * stack->dataSize)), elem, stack->dataSize);
            stack->size++;
      }
}

int stackIsEmpty(t_stack* stack) {
      return stack->size == 0;
}

void stackPeek(t_stack* stack, void* elem) {
      if (!(stackIsEmpty(stack))) {
            memcpy(elem, (void*)((uint64_t)stack->stack + ((stack->size - 1) * stack->dataSize)), stack->dataSize);
      }
}
