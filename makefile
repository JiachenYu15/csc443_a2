#Makefile
CC = g++

all: library.o 

library.o: library.cc library.h
	$(CC) -o $@ -c $<
	


