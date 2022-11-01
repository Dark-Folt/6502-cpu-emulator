
CC=gcc
CFLAGS=-g -Wall
OBJS=obj/cpu_6502.o obj/main_6502.o 
BIN=bin/main

all:$(BIN)

bin/main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main


obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) -r bin/* bin/*

