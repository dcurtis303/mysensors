CC=g++
CFLAGS=-g -Wall -Wextra -Wpedantic -Wno-deprecated

all: mysensors

mysensors: mysensors.cpp mysensors.h track.cpp track.h
	$(CC) $(CFLAGS) -c track.cpp
	$(CC) $(CFLAGS) mysensors.cpp track.o -lpthread -lsensors -lncurses -o mysensors

clean:
	rm mysensors