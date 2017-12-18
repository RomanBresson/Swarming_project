# extra flags pour le link
LDFLAGS = -lm

# Compilation options
CFLAGS = -Og -I/usr/include/openmpi -fopenmp
CXXFLAGS = -Og -I/usr/include/openmpi -fopenmp

CC  = gcc
CXX = g++
MPICC = mpicc
MPIXX = mpicxx

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = main

all : $(EXEC)

main: main.o
	$(MPIXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+

%.o: %.c
	$(MPICC) $(CFLAGS) -c $<

%.o: %.cpp
	$(MPIXX) $(CXXFLAGS) -c $<

clean:
		rm -f *.o $(EXEC)

