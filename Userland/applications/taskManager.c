// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <taskManager.h>
#include <syscalls.h>
#include <shell.h>
#include <chess.h>
#include <stringLib.h>

// static t_PCB processes[MAX_PROCESSES];

//carga las aplicaciones a correr y fuerza el inicio de la primera en la cola
void initApps() {
      runShell();
}