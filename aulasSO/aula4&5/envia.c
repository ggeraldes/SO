
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {   //numero de argumentos, argumentos

	int num, pid;

	if(argc!=3){
	 fprintf(stderr, "[ERRO] nR.de argumntos!\n    ./envia SINAL PID\n"); 
	 exit(2);
	}
	num=atoi(argv[1]);
	pid = atoi(argv[2]);
	  
#if 0
	 printf("Enviei o sinal %d ao processo %d...\n", num, pid);
	kill(pid, num);//envia o sinal (num) para o pid (oid)
#else
	 union sigval value;
	 value.sival_int=666;
	 sigqueue(pid,num,value); //envia sinal ao processo
	 printf("Enviei o sinal %d ao processo %d com o valor %d...\n", num, pid, value.sival_int);
#endif
	exit(0); //correu bem

}

