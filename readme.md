## Autores
- [Patrick Dey](https://github.com/patrickmdey)
- [Guido Gordyn Biello](https://github.com/ggordyn) 
- [Santos Rosati](https://github.com/srosati) 

# Manual de usuario

1. Ingresar a la carpeta ***"Toolchain"*** y ejecutar el comando `make all`.

2. En la carpeta principal, ejecutar el comando `make all`.

3. Ejecutar el programa mediante terminal ingresando `./run.sh`.

Una vez ejecutado el programa, se abrirá una ventana con la línea de comandos, donde se estará ejecutando la ***shell***. Para acceder al ***ajedrez*** , basta ejecutar el comando ***chess***. Para volver a la shell desde el ajedrez y guardar la partida, presionar ***tab***.


## Shell

La shell brinda una variedad de comandos, lo cuales son ejecutados al escribir su nombre y presionar ***enter***.
Los comandos son los siguientes:

- ***help:*** muestra la lista de comandos y la descripción de cada uno.
- ***inforeg:*** imprime en pantalla el valor de los registros del programa en cierto momento, para actualizar dicho valor presionar ctrl + s.
- ***time:*** imprime en pantalla el tiempo actual del sistema.
- ***cpuInfo:*** imprime en pantalla el modelo y vendedor del CPU. 
- ***cpuTemp:*** imprime en pantalla la temperatura actual del CPU. 
- ***printmem:*** dump de 32 bytes de memoria a partir de la dirección hexadecimal pasada como parámetro. 
- ***checkZeroException:*** causa una excepción de división por cero. 
- ***checkInvalidOpCodeException:*** causa una excepción de código de operación inválido.
- ***changeUsername:*** cambia el nombre del usuario de la shell al pasado como parámetro.
- ***showArgs:*** imprime en pantalla los argumentos del comando.


## Ajedrez

La aplicación del ajedrez permite ejecutar una partida de jugador humano vs jugador humano. Solamente se admiten los movimientos válidos de ajedrez, incluido el enroque. 
La ***sintaxis*** de los comandos de un movimiento está compuesta por la posición de ***origen*** y la posición de ***destino***, comenzando por la letra y luego por el número. Por ejemplo, el movimiento de una torre podría ser ***a1 a5***. El juego terminará cuando se consuma al rey enemigo o cuando el tiempo total consumido por uno de los jugadores supere en un minuto al otro.
Otros comandos:
1. Presionar R para rotar el tablero.
2. Presionar TAB para guardar la partida y volver a la shell.
3. Para realizar un enroque, deben cumplirse las condiciones tradicionales para poder realizar el movimiento, y se debe seleccionar la posición de la torre como origen y la del rey como destino.

## Syscalls

***int 80h***

| %rax | System Call       | %rdi             | %rsi              | %rdx              | %r10                | %r8 | %r9 |
| ---- | ----------------- | ---------------- | ----------------- | ----------------- | ------------------- | --- | --- |
| 0    | sys_getMem        | uint64_t memDir  | uint64_t* memData |                   |                     |     |     |
| 1    | sys_rtcTime       | t_timeInfo tInfo |                   |                   |                     |     |     |
| 2    | sys_temp          |                  |                   |                   |                     |     |     |
| 3    | sys_write         | char * string    | uint8_t length    | t_colour bgColour | t_colour fontColour |     |     |
| 4    | sys_getchar¹      |                  |                   |                   |                     |     |     |
| 5    | sys_clear²        | uint64_t fromW   | uint64_t fromH    | uint64_t toW      | uint64_t toH        |     |     |
| 6    | sys_loadApp       | t_PCB * app      |                   |                   |                     |     |     |
| 7    | sys_run           |                  |                   |                   |                     |     |     |
| 8    | sys_exit          |                  |                   |                   |                     |     |     |
| 9    | sys_inforeg       |                  |                   |                   |                     |     |     |
| 10   | sys_draw          | char * bitmap    | t_colour colour   |uint32_t scale     |                     |     |     |
| 11   | sys_moveCursor    | uint64_t offsetX | uint64_t offsetY  |                   |                     |     |     |
| 12   | sys_moveCursorTo  | uint64_t posX    | uint64_t posY     |                   |                     |     |     |
| 13   | sys_currentCursor |                  |                   |                   |                     |     |     |
| 14   | sys_ticksElapsed  |                  |                   |                   |                     |     |     |

¹si en rdi se recibe 1, se llama a getCharOnce, que solo espera un keycode una sola vez
²si en inicio y fin se recibe 0, se limpia toda la pantalla

***int 81h***

| %rax | System Call   | %rdi | %rsi | %rdx | %r10 | %r8 | %r9 |
| ---- | ------------- | ---- | ---- | ---- | ---- | --- | --- |
| 0    | sys_changeApp |      |      |      |      |     |     |
