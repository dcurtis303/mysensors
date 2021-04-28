CC=g++
CFLAGS=-g -Wall -Wextra -Wpedantic -Wno-deprecated
LDFLAGS= -lpthread -lsensors -lncurses -lconfig++

src = $(wildcard *.cpp *.h)
obj = $(src:.cpp=.o)

mysensors: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm mysensors *.o