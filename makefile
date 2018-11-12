#Makefile
CC := g++
CXXFLAGS := -std=c++11

all: library.o  test_fixed_len_sizeof

library.o: library.cc library.h
	$(CC) -o $@ -c $<

test_fixed_len_sizeof: test_fixed_len_sizeof.cc library.o
	$(CC) $(CXXFLAGS) -o $@ $< library.o
