include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin
MODULE=0000-sampleCodeModule.bin

SOURCES= $(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_LIBRARIES=$(wildcard libraries/*.c)
SOURCES_APPLICATIONS=$(wildcard applications/*.c applications/shell/*.c applications/chess/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_LIBRARIES=$(SOURCES_LIBRARIES:.c=.o)
OBJECTS_APPLICATIONS=$(SOURCES_APPLICATIONS:.c=.o)

all: $(MODULE) sampleDataModule

$(MODULE): $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_LIBRARIES) $(OBJECTS_APPLICATIONS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o $(MODULE) $^
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o 0000-sampleCodeModule.elf $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)
clean:
	rm -rf *.o applications/*.o applications/chess/*.o applications/shell/*.o asm/*.o *.bin libraries/*.o


.PHONY: sampleCodeModule all clean
