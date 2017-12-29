# extra flags pour le link
LDFLAGS = -lm 

# Compilation options
CFLAGS = -I/usr/include/openmpi -O1
CXXFLAGS = -I/usr/include/openmpi -O1

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
		rm -f *.o

mrproper: clean
		rm $(EXEC)
