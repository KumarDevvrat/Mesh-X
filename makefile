# A simple make file for complilation 
# Useful in vim with :make
CC = gcc
CFLAGS = -Wall -std=c99 
SOURCES = $(wildcard src/*.c)
EXECUTABLE = bin/mesh-x
DBGFLAGS = -g
PRFFLAGS = -pg
OPTIMIZE = -O3

all:
	$(CC) $(CFLAGS) $(OPTIMIZE) $(SOURCES) -o $(EXECUTABLE)

debug:
	$(CC) $(CFLAGS) $(DBGFLAGS) $(SOURCES) -o $(EXECUTABLE)

# To be used with gprof. Emits gmon.out
profile:
	$(CC) $(CFLAGS) $(PRFFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm -f bin/* 
