default: prog2

buildrooms.o: ngoken.buildrooms.c
	gcc -std=c89 -c ngoken.buildrooms.c -o buildrooms.o

adventure.o: ngoken.adventure.c
	gcc -std=c89 -c ngoken.adventure.c -o adventure.o -lpthread

prog2: buildrooms.o adventure.o
 	gcc buildrooms.o adventure.o -o prog2	

clean:
	rm *.o
	rm prog2