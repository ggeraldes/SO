CC=gcc


frontend: frontend.c header.h
	gcc frontend.c -o frontend

backend: backend.c header.h
	gcc backend.c -o backend

all: frontend backend

clean:
	rm backend
	rm frontend
	

