CC=gcc-5
MACCC=gcc
CFLAGS= -Wall -Wextra -pedantic -O0 -g -lm -Wno-unused-variable -Wno-unused-parameter
NASM=nasm
NASMFLAGS=-f elf64 -g -F DWARF
MACFLAGS = -g -lm  -Wno-deprecated
LIBS = -lGL -lGLU -lglut -lpthread -lm

all: demo

demo: demo.c solver.o bmp.o main_asm.o
	$(CC) $(CFLAGS) $^ -o $@  $(LIBS)

solver.o: solver.c
	$(CC) $(CFLAGS) -c $< -o $@

main_asm.o: src/main.asm
	$(NASM) $(NASMFLAGS) $< -o $@

bmp.o: bmp/bmp.c bmp/bmp.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: src/main.asm
	$(NASM) $(NASMFLAGS) $< -o $@

clean:
	rm -f *.o
	rm demo