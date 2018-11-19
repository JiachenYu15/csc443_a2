#Makefile
CC := g++
CXXFLAGS := -std=c++11

all: library.o csv2heapfile scan insert update delete select csv2colstore

library.o: library.cc library.h
	$(CC) $(CXXFLAGS) -o $@ -c $<

csv2heapfile: csv2heapfile.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

scan: scan.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

insert: insert.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

update: update.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

delete: delete.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

select: select.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o

csv2colstore: csv2colstore.cc library.h
	$(CC) $(CXXFLAGS) -o $@ $< library.o
