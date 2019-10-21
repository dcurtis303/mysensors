all: mysensors enumfeature

mysensors: mysensors.c 
	gcc -g -Wall mysensors.c -lpthread -lsensors -lncurses -o mysensors

enumfeature: enumfeature.c
	gcc -Wall enumfeature.c -lsensors -o enumfeature

clean:
	rm mysensors enumfeature