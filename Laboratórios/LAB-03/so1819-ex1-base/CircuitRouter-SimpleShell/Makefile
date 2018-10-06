# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2018-19

SOURCES = ../CircuitRouter-SeqSolver/router.c ../CircuitRouter-SeqSolver/maze.c ../CircuitRouter-SeqSolver/grid.c ../CircuitRouter-SeqSolver/coordinate.c CircuitRouter-SimpleShell.c
SOURCES+= ../lib/vector.c ../lib/queue.c ../lib/list.c ../lib/pair.c
OBJS = $(SOURCES:%.c=%.o)
CC   = gcc
CFLAGS =-g -Wall -std=gnu99 -I../
LDFLAGS=-lm
TARGET=CircuitRouter-SeqSolver
NEWTARGET = CircuitRouter-SimpleShell

all: $(NEWTARGET)

$(NEWTARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(NEWTARGET) $(LDFLAGS)

CircuitRouter-SimpleShell.o: CircuitRouter-SimpleShell.c CircuitRouter-SimpleShell.h ../CircuitRouter-SeqSolver/maze.h ../CircuitRouter-SeqSolver/router.h ../lib/list.h ../lib/timer.h ../lib/types.h
../CircuitRouter-SeqSolver/router.o: ../CircuitRouter-SeqSolver/router.c ../CircuitRouter-SeqSolver/router.h ../CircuitRouter-SeqSolver/coordinate.h ../CircuitRouter-SeqSolver/grid.h ../lib/queue.h ../lib/vector.h
../CircuitRouter-SeqSolver/maze.o: ../CircuitRouter-SeqSolver/maze.c ../CircuitRouter-SeqSolver/maze.h ../CircuitRouter-SeqSolver/coordinate.h ../CircuitRouter-SeqSolver/grid.h ../lib/list.h ../lib/queue.h ../lib/pair.h ../lib/types.h ../lib/vector.h
../CircuitRouter-SeqSolver/grid.o: ../CircuitRouter-SeqSolver/grid.c ../CircuitRouter-SeqSolver/grid.h ../CircuitRouter-SeqSolver/coordinate.h ../lib/types.h ../lib/vector.h
../CircuitRouter-SeqSolver/coordinate.o: ../CircuitRouter-SeqSolver/coordinate.c ../CircuitRouter-SeqSolver/coordinate.h ../lib/pair.h ../lib/types.h
../lib/vector.o: ../lib/vector.c ../lib/vector.h ../lib/types.h ../lib/utility.h
../lib/queue.o: ../lib/queue.c ../lib/queue.h ../lib/types.h
../lib/list.o: ../lib/list.c ../lib/list.h ../lib/types.h
../lib/pair.o: ../lib/pair.c ../lib/pair.h

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -f $(OBJS) $(TARGET)
	rm -f $(OBJS) $(NEWTARGET)
