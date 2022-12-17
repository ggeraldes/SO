CC=gcc


frontendo: frontend.c header.h
	gcc frontend.c -c

backendo: backend.c header.h
	gcc backend.c -c

frontend: frontendo
	gcc frontend.o users_lib.o -o frontend

backend: backendo
	gcc backend.c users_lib.o -o backend

all: frontend backend

clean:
	rm backend
	rm frontend
	rm frontend.o
	rm backend.o
	

