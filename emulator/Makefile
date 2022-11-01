
CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BIN=bin/main

all:$(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@


$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -r bin/* $(OBJ)/*

