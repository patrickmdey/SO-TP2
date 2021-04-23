#include <taskManager.h>
#include <systemCalls.h>
#include <shell.h>
#include <chess.h>
#include <stringLib.h>

#define MAX_PROCESSES 2

// static t_PCB processes[MAX_PROCESSES];

//carga las aplicaciones a correr y fuerza el inicio de la primera en la cola
void initApps() {
      runShell();
      /*t_PCB shell = {&runShell, 0, 0, 0, BUFFER_1};
      t_PCB chess = {&runChess, 0, 0, 0, BUFFER_0};
      processes[0] = shell;
      processes[1] = chess;
      int loaded = 0;
      for (int i = 0; i < MAX_PROCESSES; i++) {
            loaded += syscall(LOAD_APP, (uint64_t)&processes[i], 0, 0, 0, 0, 0);
      }
      if(loaded!=0){
            syscall(RUN, 0, 0, 0, 0, 0, 0);
      }*/
}