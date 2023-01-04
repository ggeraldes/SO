CC=gcc

FUSERS=-D DEBUG
FITEMS=-D DEBUG
FPROMOTORES=-D DEBUG

frontendo: frontend.c header.h
	gcc frontend.c -c

backendo: backend.c header.h
	gcc backend.c -c

frontend: frontendo
	gcc frontend.o users_lib.o -o frontend -lpthread

backend: backendo
	gcc backend.o users_lib.o -o backend -lpthread ${FUSERS} ${FITEMS} ${FPROMOTORES}

all: frontend backend

clean:
	rm backend
	rm frontend
	rm frontend.o
	rm backend.o
	rm CLIENTE[[:digit:]]*

cleanB:
	rm BACKENDFIFO

cleanC:
	rm CLIENTE[[:digit:]]*

	

