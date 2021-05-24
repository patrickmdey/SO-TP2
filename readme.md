## Autores
- [Patrick Dey](https://github.com/patrickmdey)
- [Guido Gordyn Biello](https://github.com/ggordyn) 
- [Santos Rosati](https://github.com/srosati) 

# Manual de usuario

1. Ingresar a la carpeta ***"Toolchain"*** y ejecutar el comando `make all`.

2. En la carpeta principal, ejecutar el comando `make all` acompañado de la palabra `buddy` si se quiere utilizar el administrador de memoria Buddy System.

3. Ejecutar el programa mediante terminal ingresando `./run.sh`.

Una vez ejecutado el programa, se abrirá una ventana con la línea de comandos, donde se estará ejecutando la ***shell***. Para acceder al ***ajedrez*** , basta ejecutar el comando ***chess***. Para volver a la shell desde el ajedrez y guardar la partida, presionar ***tab***.


## Shell

La shell brinda una variedad de comandos, lo cuales son ejecutados al escribir su nombre y presionar ***enter***.
Los comandos son los siguientes:

- ***help*** shows the list of commands and their use
- ***checkZeroExceptioncheckZeroException:*** triggers a zero division exception
- ***checkInvalidOpcodeException:*** triggers an invalid opcode exception
- ***inforeg:*** prints the value of all the registers on screen, press ctrl + s to update values
- ***printmem:*** recieves an hexadecimal direction and makes a memory dump of 32 bytes on screen
- ***time:*** prints the current system time on screen
- ***cpuInfo:*** prints the processor brand and model on screen
- ***cpuTemp:*** prints the current processor temperature on screen
- ***showArgs:*** prints the arguments passed to this command
- ***memoryInfo:*** prints information of memory manager status
- ***ps:*** prints a list with all running processes with their most relevant information
- ***loop:*** creates loop process
- ***kill:*** kills the process with the given pid
- ***block:*** changes process state between blocked and ready with given pid
- ***nice:*** changes the process with the given pid priority to the new priority
- ***test_sync:*** tests syncronization using semaphores
- ***test_no_sync:*** tests syncronization without using semaphores
- ***test_prio:*** tests changing processes using priority
- ***test_processes:*** tests changing processes
- ***test_mm:*** tests memory manager
- ***testContextSwitching:*** tests kernel context switching with priorities
- ***test:*** tests memory manager
- ***sem:*** prints a list with all opened semaphores with their most relevant information
- ***cat:*** prints to stdout the content of the fd
- ***filter:*** prints the vocals to stdout the content of the fd
- ***wc:*** counts the amount of lines in a given input
- ***phylo:*** simulates the phylosopher's table problem
- ***pipeInfo:*** prints a list with all opened pipes with their most relevant information'

## Tests
Entre los comandos que ofrece la shell, se encuentrar algunos testeos para verificar el buen funcionamiento de las implementaciones realizadas. Entre ellos podemos destacar los siguientes:

- ***test_sync:*** tests syncronization using semaphores
- ***test_no_sync:*** tests syncronization without using semaphores
- ***test_prio:*** tests changing processes using priority
- ***test_processes:*** tests changing processes
- ***test_mm:*** tests memory manager
- ***testContextSwitching:*** tests kernel context switching with priorities
