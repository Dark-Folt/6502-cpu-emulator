
CC=gcc
CFLAGS=-g -Wall -Wunused-variable -Werror
SRC=src
OBJ=obj
INC=include
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BIN=bin/main


TEST=tests
MACOSLIB=-I /opt/homebrew/include -L /opt/homebrew/lib
TESTLIB=-lcriterion
TOBJS=$(OBJ)/cpu_6502.o
TESTS=$(wildcard $(TEST)/*.c)
TESTBINS=$(patsubst $(TEST)/%.c, $(TEST)/bin/%, $(TESTS))

all:$(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c $(INC)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


$(TEST)/bin/%: $(TESTS) $(TOBJS)
	$(CC) $(MACOSLIB) $(TESTLIB) $^ -o $@ 

test: $(TEST)/bin $(TESTBINS) 
	for test in $(TESTBINS); do ./$$test; done


clean:
	rm -r bin/* $(OBJ)/* $(TEST)/bin/*

