CC=gcc

frontend: frontend.o
	gcc frontend.o -o frontend

frontend.o: frontend.c header.h
	gcc frontend.c -c

backend: backend.o
	gcc backend.o users_lib.o -o backend

backend.o: backend.c header.h
	gcc backend.c -c

all: frontend backend

clean:
	rm backend.o
	rm frontend.o
	

