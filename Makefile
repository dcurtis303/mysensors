all: mysensors enumfeature

mysensors: mysensors.cpp mysensors.h track.cpp track.h
	g++ -g -Wall -c track.cpp
	g++ -g -Wall mysensors.cpp track.o -lpthread -lsensors -lncurses -o mysensors

enumfeature: enumfeature.c
	gcc -Wall enumfeature.c -lsensors -o enumfeature

clean:
	rm mysensors enumfeature