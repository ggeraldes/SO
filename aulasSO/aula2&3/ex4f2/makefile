
CC=gcc

ex3: programa.o imprime.o
	gcc programa.o imprime.o -o ex3

programa.o: programa.c imprime.h
	gcc programa.c -c

imprime.o: imprime.c
	gcc imprime.c -c

limpa:
	rm *.o
