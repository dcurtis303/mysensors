all: mysensors enumfeature

mysensors: mysensors.cpp mysensors.h
	g++ -g -Wall mysensors.cpp -lpthread -lsensors -lncurses -o mysensors

enumfeature: enumfeature.c
	gcc -Wall enumfeature.c -lsensors -o enumfeature

clean:
	rm mysensors enumfeature