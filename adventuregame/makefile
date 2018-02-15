default: prog2

prog2.o: ngoken.buildrooms.c
	gcc -std=c89 -c ngoken.buildrooms.c -o prog2.o

prog2: prog2.o
	gcc prog2.o -o prog2	

clean:
	rm prog2.o
	rm prog2