#Makefile
CC := g++
CXXFLAGS := -std=c++11

all: library.o  test_fixed_len_sizeof csv2heapfile scan insert

library.o: library.cc library.h
	$(CC) $(CXXFLAGS) -o $@ -c $<

test_fixed_len_sizeof: test_fixed_len_sizeof.cc library.o
	$(CC) $(CXXFLAGS) -o $@ $< library.o

csv2heapfile: csv2heapfile.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

scan: scan.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

insert: insert.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o
